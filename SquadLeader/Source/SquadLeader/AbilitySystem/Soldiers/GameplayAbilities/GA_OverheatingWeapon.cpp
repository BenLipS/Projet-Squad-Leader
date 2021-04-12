#include "GA_OverheatingWeapon.h"
#include "../../../Soldiers/Soldier.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UGA_OverheatingWeapon::UGA_OverheatingWeapon() : TimeOverHeat { 10.f }, TimeBetweenShootMultiplier { 0.333f }
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::None;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.OverheatingWeapon")));
}

bool UGA_OverheatingWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayTagContainer* _SourceTags, const FGameplayTagContainer* _TargetTags, OUT FGameplayTagContainer* _OptionalRelevantTags) const
{
	ASoldier* SourceSoldier = Cast<ASoldier>(_ActorInfo->AvatarActor);
	return SourceSoldier && Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon()) && Super::CanActivateAbility(_Handle, _ActorInfo, _SourceTags, _TargetTags, _OptionalRelevantTags);
}

void UGA_OverheatingWeapon::ActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayAbilityActivationInfo _ActivationInfo, const FGameplayEventData* _TriggerEventData)
{
	if (!CommitAbility(_Handle, _ActorInfo, _ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ASoldier* SourceSoldier = Cast<ASoldier>(_ActorInfo->AvatarActor);
	SourceWeapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());

	SourceWeapon->ReloadWeapon();
	SourceWeapon->SetHasInfiniteAmmo(true);
	SourceWeapon->SetTimeBetweenShots(SourceWeapon->GetTimeBetweenShots() * TimeBetweenShootMultiplier);

	//UAbilityTask_WaitDelay* TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, TimeOverHeat);
	//TaskWaitDelay->Activate();
	//TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_OverheatingWeapon::EndOverHeat);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_OverheatingWeapon::EndOverHeat()
{
	SourceWeapon->SetHasInfiniteAmmo(false);
	SourceWeapon->SetTimeBetweenShots(SourceWeapon->GetTimeBetweenShots() / TimeBetweenShootMultiplier);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}