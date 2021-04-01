#include "GA_SpawnPhysicalShield.h"
#include "../../../Soldiers/Soldier.h"
#include "../AbilitySystemSoldier.h"
#include "SquadLeader/Weapons/Shield.h"

UGA_SpawnPhysicalShield::UGA_SpawnPhysicalShield()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::Ability1;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.SpawnPhysicalShield")));
}

bool UGA_SpawnPhysicalShield::CanActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayTagContainer* _SourceTags, const FGameplayTagContainer* _TargetTags, OUT FGameplayTagContainer* _OptionalRelevantTags) const
{
	return Cast<ASoldier>(_ActorInfo->AvatarActor.Get()) && Super::CanActivateAbility(_Handle, _ActorInfo, _SourceTags, _TargetTags, _OptionalRelevantTags);
}

void UGA_SpawnPhysicalShield::ActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayAbilityActivationInfo _ActivationInfo, const FGameplayEventData* _TriggerEventData)
{
	if (!CommitAbility(_Handle, _ActorInfo, _ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = CurrentActorInfo->AvatarActor.Get();
	SpawnInfo.Instigator = CurrentActorInfo->AvatarActor.Get()->GetInstigator();
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector Location = CurrentActorInfo->AvatarActor->GetActorLocation();
	Location.Z = 0.0f; // TODO: Can t be that

	AShield* Shield = GetWorld()->SpawnActor<AShield>(ShieldClass, CurrentActorInfo->AvatarActor->GetActorLocation(), CurrentActorInfo->AvatarActor->GetActorForwardVector().Rotation(), SpawnInfo);

	Shield->SetLifeSpan(5.f);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
