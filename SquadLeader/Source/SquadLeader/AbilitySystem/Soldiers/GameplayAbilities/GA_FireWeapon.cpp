#include "GA_FireWeapon.h"
#include "../../../Soldiers/Soldier.h"
#include "../AbilitySystemSoldier.h"
#include "GA_FireWeaponInstant.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UGA_FireWeapon::UGA_FireWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::BasicAttack;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon")));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Firing")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.ReloadingWeapon")));
}

void UGA_FireWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ASoldier* SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	SourceWeapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());

	UAbilitySystemSoldier* ASC = Cast<UAbilitySystemSoldier>(SourceSoldier->GetAbilitySystemComponent());

	if (!ASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	InstantAbilityHandle = ASC->FindAbilitySpecHandleForClass(GA_FireWeaponInstantClass, SourceWeapon);
	GA_FireWeaponInstantInstance = Cast<UGA_FireWeaponInstant>(ASC->GetPrimaryAbilityInstanceFromHandle(InstantAbilityHandle));

	if (!GA_FireWeaponInstantInstance)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (SourceWeapon->HasAmmo() || SourceWeapon->HasInfiniteAmmo())
		HandleFire();
	else
		ReloadWeapon();
}

bool UGA_FireWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	ASoldier* SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	return SourceSoldier && Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());
}

void UGA_FireWeapon::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UGA_FireWeapon::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	GA_FireWeaponInstantInstance->CancelAbility(InstantAbilityHandle, ActorInfo, ActivationInfo, true);
}

void UGA_FireWeapon::HandleFire()
{
	if (SourceWeapon->GetFireMode() == FGameplayTag::RequestGameplayTag(FName("Weapon.FireMode.SemiAuto")))
	{
		BatchRPCTryActivateAbility(InstantAbilityHandle, true);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else // if (SourceWeapon->GetFireMode() == FGameplayTag::RequestGameplayTag(FName("Weapon.FireMode.Automatic")))
	{
		if (!BatchRPCTryActivateAbility(InstantAbilityHandle, false))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, SourceWeapon->GetTimeBetweenShots());
		Task->Activate();
		Task->OnFinish.AddDynamic(GA_FireWeaponInstantInstance, &UGA_FireWeaponInstant::FireBullet);
	}
}

void UGA_FireWeapon::ReloadWeapon()
{
	ASoldier* SourceSoldier = Cast<ASoldier>(CurrentActorInfo->AvatarActor);
	SourceSoldier->ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.ReloadWeapon")));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}