#include "GA_Jump.h"
#include "../../../Soldiers/Soldier.h"

UGA_Jump::UGA_Jump()
{
	AbilityInputID = ESoldierAbilityInputID::Jump;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(ASoldier::SkillJumpTag);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		if (ASoldier* soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); soldier)
		{
			soldier->Jump();

			FGameplayEffectSpecHandle JumpingEffectSpecHandle = MakeOutgoingGameplayEffectSpec(JumpingGameplayEffect, GetAbilityLevel());
			soldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*JumpingEffectSpecHandle.Data.Get());
		}
	}
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	const ASoldier* soldier = CastChecked<ASoldier>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	return soldier && soldier->CanJump();
}

void UGA_Jump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
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
void UGA_Jump::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_Jump::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (ASoldier* Soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); Soldier)
	{
		Soldier->StopJumping();

		// Jumping state is kept as long as the soldier is in air - See Landed method in Soldier
		/*FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(ASoldier::StateJumpingTag);
		soldier->GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(EffectTagsToRemove);
		*/
	}
}