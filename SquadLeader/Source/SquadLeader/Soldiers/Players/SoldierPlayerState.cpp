#include "SoldierPlayerState.h"

ASoldierPlayerState::ASoldierPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemSoldier>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAttributeSetSoldier>(TEXT("Attribute Set"));
}

void ASoldierPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

TSubclassOf<ASoldierTeam> ASoldierPlayerState::GetTeam()
{
	return ITeamableContainer::GetTeam();
}

bool ASoldierPlayerState::SetTeam(TSubclassOf<ASoldierTeam> _Team)
{
	return ITeamableContainer::SetTeam(_Team);
}

UAbilitySystemSoldier* ASoldierPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSetSoldier* ASoldierPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}