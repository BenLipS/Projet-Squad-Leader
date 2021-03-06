#include "GA_ReloadWeapon.h"
#include "../../../Soldiers/Soldier.h"

UGA_ReloadWeapon::UGA_ReloadWeapon()
{
	AbilityInputID = ESoldierAbilityInputID::ReloadWeapon;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(ASoldier::SkillReloadWeaponTag);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_ReloadWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		if (ASoldier* soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); soldier)
		{
			soldier->ReloadWeapon();

			FGameplayEffectSpecHandle ReloadingWeaponEffectSpecHandle = MakeOutgoingGameplayEffectSpec(ReloadingWeaponGameplayEffect, GetAbilityLevel());
			soldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*ReloadingWeaponEffectSpecHandle.Data.Get());
		}
	}
}

bool UGA_ReloadWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	ASoldier *Soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get());
	return !!Soldier;
}

void UGA_ReloadWeapon::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_ReloadWeapon::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (ASoldier* soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); soldier)
	{
		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(ASoldier::StateReloadingWeaponTag);
		soldier->GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(EffectTagsToRemove);
	}
}