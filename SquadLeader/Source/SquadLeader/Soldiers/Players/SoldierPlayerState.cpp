#include "SoldierPlayerState.h"
#include "SoldierPlayerController.h"

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
	InitializeAttributeChangeCallbacks();
}

void ASoldierPlayerState::InitializeAttributeChangeCallbacks()
{
	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ASoldierPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ASoldierPlayerState::MaxHealthChanged);
		ShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetShieldAttribute()).AddUObject(this, &ASoldierPlayerState::ShieldChanged);
		MaxShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxShieldAttribute()).AddUObject(this, &ASoldierPlayerState::MaxShieldChanged);
	}
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

float ASoldierPlayerState::GetHealth() const
{
	return AttributeSet->GetHealth();
}

float ASoldierPlayerState::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

float ASoldierPlayerState::GetShield() const
{
	return AttributeSet->GetShield();
}

float ASoldierPlayerState::GetMaxShield() const
{
	return AttributeSet->GetMaxShield();
}

void ASoldierPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void ASoldierPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void ASoldierPlayerState::ShieldChanged(const FOnAttributeChangeData& Data)
{
	OnShieldChanged.Broadcast(Data.NewValue);
}

void ASoldierPlayerState::MaxShieldChanged(const FOnAttributeChangeData& Data)
{
	OnMaxShieldChanged.Broadcast(Data.NewValue);
}

void ASoldierPlayerState::BroadCastAllDatas()
{
	OnHealthChanged.Broadcast(GetHealth());
	OnMaxHealthChanged.Broadcast(GetMaxHealth());
	OnShieldChanged.Broadcast(GetShield());
	OnMaxShieldChanged.Broadcast(GetMaxShield());
}
