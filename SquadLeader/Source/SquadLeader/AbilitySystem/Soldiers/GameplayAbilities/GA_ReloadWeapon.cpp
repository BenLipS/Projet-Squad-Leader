#include "GA_ReloadWeapon.h"
#include "../../../Soldiers/Soldier.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UGA_ReloadWeapon::UGA_ReloadWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::ReloadWeapon;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(ASoldier::SkillReloadWeaponTag);
	ActivationOwnedTags.AddTag(ASoldier::StateReloadingWeaponTag);
}

void UGA_ReloadWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	ASoldier* SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	SourceWeapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());

	// TODO: We should play a montage then give the ammo at the end of this montage
	// For now, this is a constant...
	UAbilityTask_WaitDelay* TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, 2.f);
	TaskWaitDelay->Activate();
	TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_ReloadWeapon::ReadyToReaload);
}

bool UGA_ReloadWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	const ASoldier* SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	
	if (SourceSoldier)
	{
		const ASL_Weapon* Weapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());
		return Weapon && !Weapon->IsFullAmmo();
	}
	return false;
}

void UGA_ReloadWeapon::ReadyToReaload()
{
	SourceWeapon->ReloadWeapon();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}