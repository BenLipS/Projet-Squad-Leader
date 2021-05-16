#include "GA_FireWeapon.h"
#include "../../../Soldiers/Soldier.h"
#include "../../../Soldiers/Players/SoldierPlayer.h"
#include "../AbilitySystemSoldier.h"
#include "GA_FireWeaponInstant.h"
#include "SquadLeader/Soldiers/Soldier.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AkAudioEvent.h"
#include "AkGameplayStatics.h"

UGA_FireWeapon::UGA_FireWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::BasicAttack;
	AbilityID = ESoldierAbilityInputID::BasicAttack;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon")));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Weapon.Firing")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Weapon.Reloading")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.CastingSpell")));
}

void UGA_FireWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	SourceWeapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());

	UAbilitySystemSoldier* ASC = Cast<UAbilitySystemSoldier>(GetAbilitySystemComponentFromActorInfo());

	if (!ASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (SourceWeapon->HasAmmo() || SourceWeapon->HasInfiniteAmmo())
	{
		InstantAbilityHandle = ASC->FindAbilitySpecHandleForClass(GA_FireWeaponInstantClass, SourceWeapon);
		HandleFire();
		SourceSoldier->OnStartFiring();

	}
	else
		ReloadWeapon();
}

bool UGA_FireWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayTagContainer* _SourceTags, const FGameplayTagContainer* _TargetTags, OUT FGameplayTagContainer* _OptionalRelevantTags) const
{
	ASoldier* Soldier = Cast<ASoldier>(_ActorInfo->AvatarActor);
	return Soldier && Cast<ASL_Weapon>(Soldier->GetCurrentWeapon()) && Super::CanActivateAbility(_Handle, _ActorInfo, _SourceTags, _TargetTags, _OptionalRelevantTags);
}

void UGA_FireWeapon::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UGA_FireWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GA_FireWeaponInstantInstance)
		GA_FireWeaponInstantInstance->CancelAbility(InstantAbilityHandle, ActorInfo, ActivationInfo, true);

	SourceSoldier->OnStopFiring();

	if (SourceWeapon->GetFireMode() != FGameplayTag::RequestGameplayTag(FName("Weapon.FireMode.SemiAuto")) && SourceSoldier->IsA<ASoldierPlayer>() && Cast<ASoldierPlayer>(SourceSoldier)->GetClass() != SoldierClass::SUPPORT)
	{
		UAkGameplayStatics::PostEventByName("Rifle_Break", SourceWeapon);
	}


	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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
		UAbilitySystemSoldier* ASC = Cast<UAbilitySystemSoldier>(GetAbilitySystemComponentFromActorInfo());
		GA_FireWeaponInstantInstance = Cast<UGA_FireWeaponInstant>(ASC->GetPrimaryAbilityInstanceFromHandle(InstantAbilityHandle));

		if (!GA_FireWeaponInstantInstance || !BatchRPCTryActivateAbility(InstantAbilityHandle, false))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, SourceWeapon->GetTimeBetweenShots());
		Task->OnFinish.AddDynamic(GA_FireWeaponInstantInstance, &UGA_FireWeaponInstant::FireBullet);
		Task->ReadyForActivation();
	}
}

void UGA_FireWeapon::ReloadWeapon()
{
	SourceSoldier->ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.ReloadWeapon")));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}