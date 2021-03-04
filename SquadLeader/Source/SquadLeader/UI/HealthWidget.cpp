// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"

UHealthWidget::UHealthWidget(const FObjectInitializer& ObjectInitializer) : USL_UserWidget(ObjectInitializer)
{
}

void UHealthWidget::OnHealthChanged(float newValue)
{
	if (newValue > MaxHealth)
		newValue = MaxHealth;
	Health = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
}

void UHealthWidget::OnMaxHealthChanged(float newValue)
{
	if (newValue < Health)
		Health = newValue;
	MaxHealth = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
}