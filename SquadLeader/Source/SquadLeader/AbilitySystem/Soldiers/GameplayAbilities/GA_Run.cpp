#include "GA_Run.h"
#include "../../../Soldiers/Soldier.h"

UGA_Run::UGA_Run()
{
	AbilityInputID = ESoldierAbilityInputID::Run;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(ASoldier::SkillRunTag);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		if (ASoldier* soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); soldier && soldier->StartRunning())
		{
			FGameplayEffectSpecHandle RunningEffectSpecHandle = MakeOutgoingGameplayEffectSpec(RunningGameplayEffect, GetAbilityLevel());
			soldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*RunningEffectSpecHandle.Data.Get());
		}
	}
}

bool UGA_Run::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	ASoldier* soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get());
	return soldier && Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Run::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

// Epic's comment
/**
 *	Canceling an non instanced ability is tricky. Right now this works for Jump since there is nothing that can go wrong by calling
 *	StopJumping() if you aren't already jumping. If we had a montage playing non instanced ability, it would need to make sure the
 *	Montage that *it* played was still playing, and if so, to cancel it. If this is something we need to support, we may need some
 *	light weight data structure to represent 'non intanced abilities in action' with a way to cancel/end them.
 */
void UGA_Run::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_Run::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (ASoldier* soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); soldier)
	{
		soldier->StopRunning();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(ASoldier::StateRunningTag);
		soldier->GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(EffectTagsToRemove);
	}
}