// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldWidget.h"
#include "Interface/PlayerShieldDelegateInterface.h"

UShieldWidget::UShieldWidget(const FObjectInitializer& ObjectInitializer) : USL_UserWidget(ObjectInitializer)
{
}

void UShieldWidget::OnPlayerShieldChanged(float newValue)
{
	Shield = newValue;
	ProgressBarShield->SetPercent(Shield / MaxShield);
	TextShield->SetText(FText::FromString(FString::SanitizeFloat(Shield, 0) + "/" + FString::SanitizeFloat(MaxShield, 0)));
}

void UShieldWidget::OnPlayerMaxShieldChanged(float newValue)
{
	MaxShield = newValue;
	ProgressBarShield->SetPercent(Shield / MaxShield);
	TextShield->SetText(FText::FromString(FString::SanitizeFloat(Shield, 0) + "/" + FString::SanitizeFloat(MaxShield, 0)));
}

void UShieldWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (IPlayerShieldDelegateInterface* PlayerShieldDelegateInterface = Cast<IPlayerShieldDelegateInterface>(ObjectIn); PlayerShieldDelegateInterface)
	{
		PlayerShieldDelegateInterface->AddPlayerShieldDelegate(this);
	}
}