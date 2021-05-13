#include "GA_ReloadWeapon.h"
#include "../../../Soldiers/Soldier.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"

UGA_ReloadWeapon::UGA_ReloadWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::ReloadWeapon;
	AbilityID = ESoldierAbilityInputID::ReloadWeapon;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.ReloadWeapon")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Weapon.Reloading")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.CastingSpell")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon")));
}

void UGA_ReloadWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ASoldier* SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	SourceWeapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());

	if (UAnimMontage* ReloadWeaponMontage = SourceWeapon->ReloadMontage; ReloadWeaponMontage)
	{
		UAbilityTask_PlayMontageAndWait* TaskPlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ReloadWeaponMontage, 1.0f, NAME_None, true, 1.0f);
		TaskPlayMontage->OnCompleted.AddDynamic(this, &UGA_ReloadWeapon::MontageCompletedOrBlendedOut);
		TaskPlayMontage->OnBlendOut.AddDynamic(this, &UGA_ReloadWeapon::MontageCompletedOrBlendedOut);
		TaskPlayMontage->OnInterrupted.AddDynamic(this, &UGA_ReloadWeapon::MontageInterruptedOrCancelled);
		TaskPlayMontage->OnCancelled.AddDynamic(this, &UGA_ReloadWeapon::MontageInterruptedOrCancelled);
		TaskPlayMontage->ReadyForActivation();
	}
	else
	{
		UAbilityTask_WaitDelay* TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, 2.f); // In case there is no montage but we still want to reload weapon
		TaskWaitDelay->ReadyForActivation();
		TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_ReloadWeapon::MontageCompletedOrBlendedOut);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool UGA_ReloadWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	if (const ASoldier* SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor); SourceSoldier)
	{
		const ASL_Weapon* Weapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());
		return Weapon && !Weapon->IsFullAmmo();
	}
	return false;
}

void UGA_ReloadWeapon::MontageCompletedOrBlendedOut()
{
	UAbilityTask_NetworkSyncPoint* WaitNetSyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::OnlyServerWait);
	WaitNetSyncTask->OnSync.AddDynamic(this, &UGA_ReloadWeapon::ReloadWeapon);
	WaitNetSyncTask->ReadyForActivation();
}

void UGA_ReloadWeapon::MontageInterruptedOrCancelled()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_ReloadWeapon::ReloadWeapon()
{
	SourceWeapon->ReloadWeapon();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}