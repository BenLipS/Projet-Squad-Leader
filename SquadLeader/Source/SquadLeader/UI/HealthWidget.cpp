// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"
#include "Interface/PlayerShieldDelegateInterface.h"
#include "Interface/PlayerHealthDelegateInterface.h"

UHealthWidget::UHealthWidget(const FObjectInitializer& ObjectInitializer) : USL_UserWidget(ObjectInitializer)
{
}

void UHealthWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (IPlayerHealthDelegateInterface* PlayerHealthDelegateInterface = Cast<IPlayerHealthDelegateInterface>(ObjectIn); PlayerHealthDelegateInterface)
	{
		PlayerHealthDelegateInterface->AddPlayerHealthDelegate(this);
	}

	if (IPlayerShieldDelegateInterface* PlayerShieldDelegateInterface = Cast<IPlayerShieldDelegateInterface>(ObjectIn); PlayerShieldDelegateInterface)
	{
		PlayerShieldDelegateInterface->AddPlayerShieldDelegate(this);
	}
}

void UHealthWidget::OnPlayerHealthChanged(float newValue)
{
	/*if (newValue > MaxHealth)
		newValue = MaxHealth;*/
	Health = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
	//TextHP->SetText(FText::FromString(FString::SanitizeFloat(Health,0) + "/" + FString::SanitizeFloat(MaxHealth,0) + "HP"));
	//TextHP->SetText(FText::FromString(FString::SanitizeFloat(Health,0) + " + " + FString::SanitizeFloat(Shield, 0) + " HP"));
}

void UHealthWidget::OnPlayerMaxHealthChanged(float newValue)
{
	ensure(HPSegment > 0.f);
	if (newValue != MaxHealth)
	{
		MaxHealth = newValue;
	}
	ProgressBarHP->SetNbSplit(MaxHealth / HPSegment);


	ProgressBarHP->SetPercent(Health / MaxHealth);
	//TextHP->SetText(FText::FromString(FString::SanitizeFloat(Health, 0) + "/" + FString::SanitizeFloat(MaxHealth, 0) + "HP"));
}

void UHealthWidget::OnPlayerShieldChanged(float newValue)
{
	Shield = newValue;
	ProgressBarShield->SetPercent(Shield / MaxShield);
	//TextShield->SetText(FText::FromString(FString::SanitizeFloat(Shield, 0) + "/" + FString::SanitizeFloat(MaxShield, 0)));
	//TextHP->SetText(FText::FromString(FString::SanitizeFloat(Health, 0) + " + " + FString::SanitizeFloat(Shield, 0) + " HP"));
}

void UHealthWidget::OnPlayerMaxShieldChanged(float newValue)
{
	if (newValue == 0.f)
	{
		ShieldBackground->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		if (MaxShield == 0.f)
		{
			ShieldBackground->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		ProgressBarShield->SetPercent(Shield / newValue);
		ProgressBarShield->SetNbSplit(newValue / ShieldSegment);
	}

	MaxShield = newValue;
	//TextShield->SetText(FText::FromString(FString::SanitizeFloat(Shield, 0) + "/" + FString::SanitizeFloat(MaxShield, 0)));
}