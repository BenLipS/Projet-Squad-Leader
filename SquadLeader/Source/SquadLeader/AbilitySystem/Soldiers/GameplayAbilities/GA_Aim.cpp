#include "GA_Aim.h"
#include "../../../Soldiers/Soldier.h"

UGA_Aim::UGA_Aim()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::Aim;
	AbilityID = ESoldierAbilityInputID::Aim;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Aim")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Aiming")));
}

void UGA_Aim::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		ASoldier* Soldier = CastChecked<ASoldier>(ActorInfo->AvatarActor.Get());
		Soldier->StartAiming();
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool UGA_Aim::CanActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayTagContainer* _SourceTags, const FGameplayTagContainer* _TargetTags, OUT FGameplayTagContainer* _OptionalRelevantTags) const
{
	ASoldier* Soldier = Cast<ASoldier>(_ActorInfo->AvatarActor.Get());

	return Soldier && Soldier->GetCurrentWeapon() && Super::CanActivateAbility(_Handle, _ActorInfo, _SourceTags, _TargetTags, _OptionalRelevantTags);
}

void UGA_Aim::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Aim::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ASoldier* Soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get());
	Soldier->StopAiming();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}