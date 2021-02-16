#include "SoldierAI.h"

ASoldierAI::ASoldierAI() : ASoldier()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemSoldier>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAttributeSetSoldier>(TEXT("Attribute Set"));
}

void ASoldierAI::BeginPlay()
{
	Super::BeginPlay();

	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
	InitializeAbilities();
	AddStartupEffects();
	InitializeTagChangeCallbacks();
	initWeapons();
}

bool ASoldierAI::StartFiring()
{
	FGameplayTagContainer shootTag;
	shootTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon")));
	return GetAbilitySystemComponent()->TryActivateAbilitiesByTag(shootTag);
}

void ASoldierAI::StopFiring()
{
	FGameplayTagContainer shootTag;
	shootTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon")));
	GetAbilitySystemComponent()->CancelAbilities(&shootTag);
}