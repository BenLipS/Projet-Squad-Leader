#include "GA_QuickDash.h"
#include "../../../Soldiers/Soldier.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"

UGA_QuickDash::UGA_QuickDash()
{
	AbilityInputID = ESoldierAbilityInputID::QuickDash;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.QuickDash")));
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_QuickDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		if (ASoldier* Soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); Soldier)
		{
			FVector Direction = Soldier->GetVelocity().IsNearlyZero() ? Soldier->GetActorForwardVector() : Soldier->GetVelocity();

			UAbilityTask_ApplyRootMotionConstantForce* Task = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, FName("Apply Root Motion Constant Force"), Direction, Strength, Duration, bIsAdditive, nullptr, ERootMotionFinishVelocityMode::SetVelocity, Soldier->GetVelocity(), 0.f, true);
			
			Task->OnFinish.AddDynamic(this, &UGA_QuickDash::OnDashEnded);
			Task->ReadyForActivation();
		}
	}
}

bool UGA_QuickDash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	ASoldier* Soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get());
	return Soldier && Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_QuickDash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_QuickDash::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	// TODO: State dashing  or shall we make the damages impossible ?
	/*if (ASoldier* Soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); Soldier)
	{
		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Running")));
		Soldier->GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(EffectTagsToRemove);
	}*/
}

void UGA_QuickDash::OnDashEnded()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}