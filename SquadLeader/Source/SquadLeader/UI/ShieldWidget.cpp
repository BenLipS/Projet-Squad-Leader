// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldWidget.h"

UShieldWidget::UShieldWidget(const FObjectInitializer& ObjectInitializer) : USL_UserWidget(ObjectInitializer)
{
}

void UShieldWidget::OnShieldChanged(float newValue)
{
	/*if (newValue > MaxShield)
		newValue = MaxShield;*/
	Shield = newValue;
	ProgressBarShield->SetPercent(Shield / MaxShield);
	TextShield->SetText(FText::FromString(FString::SanitizeFloat(Shield, 0) + "/" + FString::SanitizeFloat(MaxShield, 0)));
}

void UShieldWidget::OnMaxShieldChanged(float newValue)
{
	/*if (newValue < Shield)
		Shield = newValue;*/
	MaxShield = newValue;
	ProgressBarShield->SetPercent(Shield / MaxShield);
	TextShield->SetText(FText::FromString(FString::SanitizeFloat(Shield, 0) + "/" + FString::SanitizeFloat(MaxShield, 0)));
}