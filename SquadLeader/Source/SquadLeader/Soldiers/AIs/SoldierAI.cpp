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

bool ASoldierAI::ActivateAbilities(const FGameplayTagContainer &_TagContainer)
{
	return AbilitySystemComponent->TryActivateAbilitiesByTag(_TagContainer);
}

bool ASoldierAI::ActivateAbility(const FGameplayTag &_Tag)
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(_Tag);
	return AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
}

void ASoldierAI::CancelAbilities(const FGameplayTagContainer &_TagContainer)
{
	AbilitySystemComponent->CancelAbilities(&_TagContainer);
}

void ASoldierAI::CancelAbility(const FGameplayTag &_Tag)
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(_Tag);
	AbilitySystemComponent->CancelAbilities(&TagContainer);
}

bool ASoldierAI::ActivateAbilityFire()
{
	FGameplayTagContainer shootTag;
	shootTag.AddTag(ASoldier::SkillFireWeaponTag);
	return AbilitySystemComponent->TryActivateAbilitiesByTag(shootTag);
}

void ASoldierAI::CancelAbilityFire()
{
	FGameplayTagContainer shootTag;
	shootTag.AddTag(ASoldier::SkillFireWeaponTag);
	AbilitySystemComponent->CancelAbilities(&shootTag);
}

bool ASoldierAI::ActivateAbilityRun()
{
	FGameplayTagContainer runTag;
	runTag.AddTag(ASoldier::SkillRunTag);
	return AbilitySystemComponent->TryActivateAbilitiesByTag(runTag);
}

void ASoldierAI::CancelAbilityRun()
{
	FGameplayTagContainer runTag;
	runTag.AddTag(ASoldier::SkillRunTag);
	AbilitySystemComponent->CancelAbilities(&runTag);
}