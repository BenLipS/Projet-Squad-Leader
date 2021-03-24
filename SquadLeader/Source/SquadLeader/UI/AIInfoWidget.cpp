// Fill out your copyright notice in the Description page of Project Settings.


#include "AIInfoWidget.h"

UAIInfoWidget::UAIInfoWidget(const FObjectInitializer& ObjectInitializer) : USL_UserWidget(ObjectInitializer)
{
}

void UAIInfoWidget::OnHealthChanged(float newValue)
{
	Health = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
	TextAI->SetText(FText::FromString(FString::SanitizeFloat(Health, 0)));
}

void UAIInfoWidget::OnMaxHealthChanged(float newValue)
{
	MaxHealth = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
}

void UAIInfoWidget::OnShieldChanged(float newValue)
{
	Shield = newValue;
	ProgressBarShield->SetPercent(Shield / MaxShield);
}

void UAIInfoWidget::OnMaxShieldChanged(float newValue)
{
	MaxShield = newValue;
	ProgressBarShield->SetPercent(Shield / MaxShield);
}