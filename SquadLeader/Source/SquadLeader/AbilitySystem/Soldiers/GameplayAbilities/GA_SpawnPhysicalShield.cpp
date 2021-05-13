#include "GA_SpawnPhysicalShield.h"
#include "../../../Soldiers/Soldier.h"
#include "../AbilitySystemSoldier.h"
#include "SquadLeader/Weapons/Shield.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "SquadLeader/AbilitySystem/Soldiers/AbilityTasks/AbilityTask_PlayMontageAndWaitForEvent.h"
//#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_SpawnPhysicalShield::UGA_SpawnPhysicalShield() :
ShieldLifeSpan{ 5.f },
ShieldHealth { 500.f },
ShieldDistanceFromCaller{ 0.f },
ShieldScale{ FVector{ 1.f, 1.f, 1.f } }
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.SpawnPhysicalShield")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.CastingSpell.ActivatingPhysicalShield")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.CastingSpell")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.ReloadWeapon")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon")));
	SpawnShieldEventTag = FGameplayTag::RequestGameplayTag(FName("Event.Ability.ShieldSpawned"));
}

bool UGA_SpawnPhysicalShield::CanActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayTagContainer* _SourceTags, const FGameplayTagContainer* _TargetTags, OUT FGameplayTagContainer* _OptionalRelevantTags) const
{
	return Cast<ASoldier>(_ActorInfo->AvatarActor.Get()) && Super::CanActivateAbility(_Handle, _ActorInfo, _SourceTags, _TargetTags, _OptionalRelevantTags);
}

void UGA_SpawnPhysicalShield::ActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayAbilityActivationInfo _ActivationInfo, const FGameplayEventData* _TriggerEventData)
{
	if (!CommitAbility(_Handle, _ActorInfo, _ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ASoldier* SourceSoldier = Cast<ASoldier>(_ActorInfo->AvatarActor);

	if (UAnimMontage* CastSpellMontage = SourceSoldier->CastSpellMontage; CastSpellMontage)
	{
		FGameplayTagContainer Tags;
		Tags.AddTag(SpawnShieldEventTag);

		UAbilityTask_PlayMontageAndWaitForEvent* TaskPlayMontage = UAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, CastSpellMontage, Tags);

		TaskPlayMontage->OnCompleted.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageCompletedOrBlendedOut);
		TaskPlayMontage->OnBlendOut.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageCompletedOrBlendedOut);
		TaskPlayMontage->OnInterrupted.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageInterruptedOrCancelled);
		TaskPlayMontage->OnCancelled.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageInterruptedOrCancelled);
		TaskPlayMontage->OnEventReceived.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageSentEvent);
		TaskPlayMontage->ReadyForActivation();
	}
	else
	{
		UAbilityTask_WaitDelay* TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, 1.f); // In case there is no montage but we still want to spawn the shield
		TaskWaitDelay->ReadyForActivation();
		TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageCompletedOrBlendedOut);
	}

	Super::ActivateAbility(_Handle, _ActorInfo, _ActivationInfo, _TriggerEventData);
}

void UGA_SpawnPhysicalShield::MontageCompletedOrBlendedOut()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SpawnPhysicalShield::MontageInterruptedOrCancelled()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_SpawnPhysicalShield::MontageSentEvent(FGameplayTag _EventTag, FGameplayEventData _EventData)
{
	if (_EventTag == SpawnShieldEventTag)
	{
		if (CurrentActorInfo->IsNetAuthority())
			SpawnShield();

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	}
}

void UGA_SpawnPhysicalShield::SpawnShield()
{
	ASoldier* SourceSoldier = Cast<ASoldier>(CurrentActorInfo->AvatarActor);

	FTransform Transform{ SourceSoldier->GetActorForwardVector().Rotation(), SourceSoldier->GetActorLocation() + ShieldDistanceFromCaller * SourceSoldier->GetActorForwardVector(), ShieldScale };
	AShield* Shield = GetWorld()->SpawnActorDeferred<AShield>(ShieldClass, Transform, SourceSoldier, SourceSoldier, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Shield)
	{
		Shield->SetLifeSpan(ShieldLifeSpan);
		Shield->SetHealth(ShieldHealth);

		if (SourceSoldier && SourceSoldier->GetTeam() && SourceSoldier->GetTeam()->Id == 1)
			Shield->SetCollisionProfile(TEXT(PN_Shield1));
		else
			Shield->SetCollisionProfile(TEXT(PN_Shield2));

		Shield->FinishSpawning(Transform);
	}
}