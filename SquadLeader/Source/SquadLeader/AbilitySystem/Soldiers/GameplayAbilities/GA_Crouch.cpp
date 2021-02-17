#include "GA_Crouch.h"
#include "../../../Soldiers/Soldier.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Crouch::UGA_Crouch()
{
	AbilityInputID = ESoldierAbilityInputID::Crouch;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(ASoldier::SkillCrouchTag);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		ASoldier* soldier = CastChecked<ASoldier>(ActorInfo->AvatarActor.Get());

		if (soldier->StartCrouching())
		{
			FGameplayEffectSpecHandle CrouchingEffectSpecHandle = MakeOutgoingGameplayEffectSpec(CrouchingGameplayEffect, GetAbilityLevel());
			soldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*CrouchingEffectSpecHandle.Data.Get());
		}
	}
}

bool UGA_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	const ASoldier* soldier = CastChecked<ASoldier>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	return soldier && soldier->CanJump();
}

void UGA_Crouch::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
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
void UGA_Crouch::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_Crouch::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	ASoldier* soldier = CastChecked<ASoldier>(ActorInfo->AvatarActor.Get());
	soldier->StopCrouching();
}