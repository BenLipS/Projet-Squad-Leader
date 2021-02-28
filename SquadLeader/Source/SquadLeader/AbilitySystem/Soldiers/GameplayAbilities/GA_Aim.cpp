#include "GA_Aim.h"
#include "../../../Soldiers/Soldier.h"

UGA_Aim::UGA_Aim()
{
	AbilityInputID = ESoldierAbilityInputID::Aim;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(ASoldier::SkillAimTag);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Aim::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		ASoldier* soldier = CastChecked<ASoldier>(ActorInfo->AvatarActor.Get());
		soldier->StartAiming();

		FGameplayEffectSpecHandle AimingEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AimingGameplayEffect, GetAbilityLevel());
		soldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*AimingEffectSpecHandle.Data.Get());
	}
}

bool UGA_Aim::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	ASoldier* soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get());

	if (soldier && soldier->getCurrentWeapon() && Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return true;
	return false;
}

void UGA_Aim::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UGA_Aim::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_Aim::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	ASoldier* soldier = CastChecked<ASoldier>(ActorInfo->AvatarActor.Get());
	soldier->StopAiming();

	FGameplayTagContainer EffectTagsToRemove;
	EffectTagsToRemove.AddTag(ASoldier::StateAimingTag);
	soldier->GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(EffectTagsToRemove);
}