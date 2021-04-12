#include "GA_SpawnPhysicalShield.h"
#include "../../../Soldiers/Soldier.h"
#include "../AbilitySystemSoldier.h"
#include "SquadLeader/Weapons/Shield.h"

UGA_SpawnPhysicalShield::UGA_SpawnPhysicalShield() :
ShieldLifeSpan{ 5.f },
ShieldHealth { 500.f },
ShieldDistanceFromCaller{ 0.f },
ShieldScale{ FVector{ 1.f, 1.f, 1.f } }
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

	if (CurrentActorInfo->IsNetAuthority())
		SpawnShield();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_SpawnPhysicalShield::SpawnShield()
{
	ASoldier* SourceSoldier = Cast<ASoldier>(CurrentActorInfo->AvatarActor);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = SourceSoldier;
	SpawnInfo.Instigator = SourceSoldier;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector Location = CurrentActorInfo->AvatarActor->GetActorLocation();
	Location.Z -= SourceSoldier->GetDefaultHalfHeight() * 2.f;

	AShield* Shield = GetWorld()->SpawnActor<AShield>(ShieldClass, SourceSoldier->GetActorLocation() + ShieldDistanceFromCaller * SourceSoldier->GetActorForwardVector(), SourceSoldier->GetActorForwardVector().Rotation(), SpawnInfo);

	Shield->SetLifeSpan(ShieldLifeSpan);
	Shield->SetHealth(ShieldHealth);
	Shield->SetActorScale3D(ShieldScale);
}