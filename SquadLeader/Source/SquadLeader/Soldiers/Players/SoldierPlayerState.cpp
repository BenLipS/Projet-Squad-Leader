#include "SoldierPlayerState.h"

ASoldierPlayerState::ASoldierPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemSoldier>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAttributeSetSoldier>(TEXT("Attribute Set"));

	bReplicates = true;
}

void ASoldierPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ASoldierPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// everyone
	DOREPLIFETIME(ASoldierPlayerState, Team);
}

TSubclassOf<ASoldierTeam> ASoldierPlayerState::GetTeam()
{
	return Team;
}

bool ASoldierPlayerState::SetTeam(TSubclassOf<ASoldierTeam> _Team)
{
	if (GetLocalRole() == ROLE_Authority) {  // only server can change team
		if (auto soldier = Cast<ASoldier>(GetPawn()); soldier) {
			if (Team)
				Team.GetDefaultObject()->RemoveSoldierList(soldier);
			if (_Team)
				_Team.GetDefaultObject()->AddSoldierList(soldier);
		}

		Team = _Team;
		return true;
	}
	return false;
}

UAbilitySystemSoldier* ASoldierPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSetSoldier* ASoldierPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}