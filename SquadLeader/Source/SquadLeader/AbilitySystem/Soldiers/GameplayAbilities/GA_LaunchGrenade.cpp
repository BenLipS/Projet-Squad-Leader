#include "GA_LaunchGrenade.h"
#include "../../../Soldiers/Soldier.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "SquadLeader/AbilitySystem/Soldiers/AbilityTasks/AbilityTask_PlayMontageAndWaitForEvent.h"
//#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_LaunchGrenade::UGA_LaunchGrenade()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Grenade")));
	ThrownProjectileEventTag = FGameplayTag::RequestGameplayTag(FName("Event.Ability.ProjectileThrown"));
}

bool UGA_LaunchGrenade::CanActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayTagContainer* _SourceTags, const FGameplayTagContainer* _TargetTags, OUT FGameplayTagContainer* _OptionalRelevantTags) const
{
	return Cast<ASoldier>(_ActorInfo->AvatarActor.Get()) && Super::CanActivateAbility(_Handle, _ActorInfo, _SourceTags, _TargetTags, _OptionalRelevantTags);
}

void UGA_LaunchGrenade::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);

	if (UAnimMontage* ThrowProjectileMontage = SourceSoldier->ThrowProjectileMontage; ThrowProjectileMontage)
	{
		SourceSoldier->UseCurrentWeaponWithLeftHand(); // Because the animation throw with right hand

		FGameplayTagContainer Tags;
		Tags.AddTag(ThrownProjectileEventTag);

		UAbilityTask_PlayMontageAndWaitForEvent* TaskPlayMontage = UAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, ThrowProjectileMontage, Tags);

		TaskPlayMontage->OnCompleted.AddDynamic(this, &UGA_LaunchGrenade::MontageCompletedOrBlendedOut);
		TaskPlayMontage->OnBlendOut.AddDynamic(this, &UGA_LaunchGrenade::MontageCompletedOrBlendedOut);
		TaskPlayMontage->OnInterrupted.AddDynamic(this, &UGA_LaunchGrenade::MontageInterruptedOrCancelled);
		TaskPlayMontage->OnCancelled.AddDynamic(this, &UGA_LaunchGrenade::MontageInterruptedOrCancelled);
		TaskPlayMontage->OnEventReceived.AddDynamic(this, &UGA_LaunchGrenade::MontageSentEvent);
		TaskPlayMontage->ReadyForActivation();
	}
	else
	{
		UAbilityTask_WaitDelay* TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, 1.5f); // In case there is no montage but we still want to throw the grenade
		TaskWaitDelay->ReadyForActivation();
		TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_LaunchGrenade::MontageCompletedOrBlendedOut);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_LaunchGrenade::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	SourceSoldier->UseCurrentWeaponWithRightHand(); // Since it is the default config
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_LaunchGrenade::MontageCompletedOrBlendedOut()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_LaunchGrenade::MontageInterruptedOrCancelled()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_LaunchGrenade::MontageSentEvent(FGameplayTag _EventTag, FGameplayEventData _EventData)
{
	if (_EventTag == ThrownProjectileEventTag)
	{
		if (CurrentActorInfo->IsNetAuthority())
			ThrowProjectile();
	}
}

void UGA_LaunchGrenade::ThrowProjectile()
{
	// Create projectile then launch it
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = SourceSoldier;
	SpawnInfo.Instigator = SourceSoldier;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FName SocketHandName = bThrowWithRightHand ? FName{ SourceSoldier->WeaponAttachPointRightHand } : FName{ SourceSoldier->WeaponAttachPointLeftHand };
	GetWorld()->SpawnActor<ASL_Projectile>(ProjectileClass, SourceSoldier->GetMesh()->GetSocketLocation(SocketHandName), SourceSoldier->CurrentCameraComponent->GetForwardVector().Rotation(), SpawnInfo);
}