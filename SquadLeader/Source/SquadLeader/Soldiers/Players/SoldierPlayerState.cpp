#include "SoldierPlayerState.h"
#include "SoldierPlayerController.h"
#include "../../SquadLeaderGameModeBase.h"

ASoldierPlayerState::ASoldierPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemSoldier>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAttributeSetSoldier>(TEXT("Attribute Set"));

	NetUpdateFrequency = 100.0f;
}

void ASoldierPlayerState::BeginPlay()
{
	Super::BeginPlay();
	InitializeAttributeChangeCallbacks();

	PersonalRecord = NewObject<AKillStats>();
}

void ASoldierPlayerState::InitializeAttributeChangeCallbacks()
{
	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ASoldierPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ASoldierPlayerState::MaxHealthChanged);
		ShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetShieldAttribute()).AddUObject(this, &ASoldierPlayerState::ShieldChanged);
		MaxShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxShieldAttribute()).AddUObject(this, &ASoldierPlayerState::MaxShieldChanged);
		EXPChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetEXPAttribute()).AddUObject(this, &ASoldierPlayerState::EXPChanged);
		EXPLevelUpChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetEXPLevelUpAttribute()).AddUObject(this, &ASoldierPlayerState::EXPLevelUpChanged);
	}
}

ASoldierTeam* ASoldierPlayerState::GetTeam()
{
	if (auto soldier = Cast<ASoldier>(GetPawn()); soldier) {
		return soldier->GetTeam();
	}
	return nullptr;
}

bool ASoldierPlayerState::SetTeam(ASoldierTeam* _Team)
{
	if (auto soldier = Cast<ASoldier>(GetPawn()); soldier) {
		return soldier->SetTeam(_Team);
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

float ASoldierPlayerState::GetEXP() const
{
	return AttributeSet->GetEXP();
}

float ASoldierPlayerState::GetEXPLevelUp() const
{
	return AttributeSet->GetEXPLevelUp();
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

void ASoldierPlayerState::EXPChanged(const FOnAttributeChangeData& Data)
{
	OnEXPChanged.Broadcast(Data.NewValue);
}

void ASoldierPlayerState::EXPLevelUpChanged(const FOnAttributeChangeData& Data)
{
	OnEXPLevelUpChanged.Broadcast(Data.NewValue);
}

void ASoldierPlayerState::BroadCastAllDatas()
{
	OnHealthChanged.Broadcast(GetHealth());
	OnMaxHealthChanged.Broadcast(GetMaxHealth());
	OnShieldChanged.Broadcast(GetShield());
	OnMaxShieldChanged.Broadcast(GetMaxShield());
	OnEXPChanged.Broadcast(GetEXP());
	OnEXPLevelUpChanged.Broadcast(GetEXPLevelUp());
}

void ASoldierPlayerState::SetPlayerParam(APlayerParam* _PlayerParam, AController* OwningController)
{
	PlayerParam = _PlayerParam;
	PlayerParam->AdaptAllAIToTeam();

	// recreate a new pawn and possess it
	OwningController->Possess(OwningController->GetWorld()->GetAuthGameMode<ASquadLeaderGameModeBase>()->SpawnSoldier(PlayerParam, OwningController));
}

APlayerParam* ASoldierPlayerState::GetPlayerParam()
{
	return PlayerParam;
}
