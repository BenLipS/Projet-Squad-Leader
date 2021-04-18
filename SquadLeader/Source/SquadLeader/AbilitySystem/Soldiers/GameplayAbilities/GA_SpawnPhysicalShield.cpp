#include "GA_SpawnPhysicalShield.h"
#include "../../../Soldiers/Soldier.h"
#include "../AbilitySystemSoldier.h"
#include "SquadLeader/Weapons/Shield.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_SpawnPhysicalShield::UGA_SpawnPhysicalShield() :
ShieldLifeSpan{ 5.f },
ShieldHealth { 500.f },
ShieldDistanceFromCaller{ 0.f },
ShieldScale{ FVector{ 1.f, 1.f, 1.f } }
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::Ability1;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.SpawnPhysicalShield")));
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
		UAbilityTask_PlayMontageAndWait* TaskPlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CastSpellMontage, 1.0f, NAME_None, true, 1.0f);
		TaskPlayMontage->OnCompleted.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageCompletedOrBlendedOut);
		TaskPlayMontage->OnBlendOut.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageCompletedOrBlendedOut);
		TaskPlayMontage->OnInterrupted.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageInterruptedOrCancelled);
		TaskPlayMontage->OnCancelled.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageInterruptedOrCancelled);
		TaskPlayMontage->ReadyForActivation();
	}
	else
	{
		UAbilityTask_WaitDelay* TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, 1.f); // In case there is no montage but we still want to spawn the shield
		TaskWaitDelay->ReadyForActivation();
		TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_SpawnPhysicalShield::MontageCompletedOrBlendedOut);
	}
}

void UGA_SpawnPhysicalShield::MontageCompletedOrBlendedOut()
{
	if (CurrentActorInfo->IsNetAuthority())
		SpawnShield();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SpawnPhysicalShield::MontageInterruptedOrCancelled()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_SpawnPhysicalShield::SpawnShield()
{
	ASoldier* SourceSoldier = Cast<ASoldier>(CurrentActorInfo->AvatarActor);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = SourceSoldier;
	SpawnInfo.Instigator = SourceSoldier;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// TODO: What should be Z position ?
	//FVector Location = CurrentActorInfo->AvatarActor->GetActorLocation();
	//Location.Z -= SourceSoldier->GetDefaultHalfHeight() * 2.f;

	AShield* Shield = GetWorld()->SpawnActor<AShield>(ShieldClass, SourceSoldier->GetActorLocation() + ShieldDistanceFromCaller * SourceSoldier->GetActorForwardVector(), SourceSoldier->GetActorForwardVector().Rotation(), SpawnInfo);

	Shield->SetLifeSpan(ShieldLifeSpan);
	Shield->SetHealth(ShieldHealth);
	Shield->SetActorScale3D(ShieldScale);
}