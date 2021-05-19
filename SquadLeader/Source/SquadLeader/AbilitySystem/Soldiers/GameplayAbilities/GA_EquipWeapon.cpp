#include "GA_EquipWeapon.h"

#include "SquadLeader/Weapons/SL_Weapon.h"
#include "SquadLeader/Soldiers/Soldier.h"

UGA_EquipWeapon::UGA_EquipWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.EquipWeapon.Bazooka")));
}

bool UGA_EquipWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	ASoldier* Soldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	return WeaponClass && Cast<ASoldier>(ActorInfo->AvatarActor) && Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_EquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ASoldier* Soldier = Cast<ASoldier>(OwnerInfo->AvatarActor);

	Soldier->EquipWeapon(WeaponClass);
	Soldier->GetCurrentWeapon()->ReloadWeapon();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);
}