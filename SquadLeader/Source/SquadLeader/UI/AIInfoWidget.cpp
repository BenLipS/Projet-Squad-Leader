// Fill out your copyright notice in the Description page of Project Settings.


#include "AIInfoWidget.h"

UAIInfoWidget::UAIInfoWidget(const FObjectInitializer& ObjectInitializer) : USL_UserWidget(ObjectInitializer)
{
}

void UAIInfoWidget::OnHealthChanged(float newValue)
{
	if (newValue > MaxHealth)
		newValue = MaxHealth;
	Health = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
}

void UAIInfoWidget::OnMaxHealthChanged(float newValue)
{
	if (newValue < Health)
		Health = newValue;
	MaxHealth = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
}

void UAIInfoWidget::OnTextChanged(FText newText)
{
	TextAI->SetText(newText);
}