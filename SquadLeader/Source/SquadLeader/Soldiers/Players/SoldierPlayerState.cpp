#include "SoldierPlayerState.h"
#include "SoldierPlayerController.h"
#include "../../UI/PlayerHUD.h"

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
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwner()); PC)
	{
		if (auto HUD = PC->GetHUD())
		{
			if (auto PlayerHUD = Cast<APlayerHUD>(HUD))
				PlayerHUD->OnHealthChanged(Data.NewValue);
		}
	}
}

void ASoldierPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwner()); PC)
	{
		if (auto HUD = PC->GetHUD())
		{
			if (auto PlayerHUD = Cast<APlayerHUD>(HUD))
				PlayerHUD->OnMaxHealthChanged(Data.NewValue);
		}
	}
}

void ASoldierPlayerState::ShieldChanged(const FOnAttributeChangeData& Data)
{
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwner()); PC)
	{
		if (auto HUD = PC->GetHUD())
		{
			if (auto PlayerHUD = Cast<APlayerHUD>(HUD))
				PlayerHUD->OnShieldChanged(Data.NewValue);
		}
	}
}

void ASoldierPlayerState::MaxShieldChanged(const FOnAttributeChangeData& Data)
{
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwner()); PC)
	{
		if (auto HUD = PC->GetHUD())
		{
			if (auto PlayerHUD = Cast<APlayerHUD>(HUD))
				PlayerHUD->OnMaxShieldChanged(Data.NewValue);
		}
	}
}