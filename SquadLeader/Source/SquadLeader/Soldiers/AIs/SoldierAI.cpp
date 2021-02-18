#include "SoldierAI.h"

ASoldierAI::ASoldierAI(const FObjectInitializer& _ObjectInitializer) : ASoldier(_ObjectInitializer)
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

	InitializeTagChangeCallbacks();
	InitializeAttributeChangeCallbacks();
	InitializeAttributes();
	InitializeAbilities();
	AddStartupEffects();
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
	ActivateAbility(ASoldier::SkillFireWeaponTag);
}

void ASoldierAI::CancelAbilityFire()
{
	CancelAbility(ASoldier::SkillFireWeaponTag);
}

bool ASoldierAI::ActivateAbilityRun()
{
	ActivateAbility(ASoldier::SkillRunTag);
}

void ASoldierAI::CancelAbilityRun()
{
	CancelAbility(ASoldier::SkillRunTag);
}