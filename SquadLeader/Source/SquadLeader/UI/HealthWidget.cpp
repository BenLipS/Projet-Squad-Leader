// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"

UHealthWidget::UHealthWidget(const FObjectInitializer& ObjectInitializer) : USL_UserWidget(ObjectInitializer)
{
}

void UHealthWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (IPlayerHealthDelegateInterface* PlayerHealthDelegateInterface = Cast<IPlayerHealthDelegateInterface>(ObjectIn); PlayerHealthDelegateInterface)
	{
		PlayerHealthDelegateInterface->AddPlayerHealthDelegate(this);
	}
}

void UHealthWidget::OnPlayerHealthChanged(float newValue)
{
	/*if (newValue > MaxHealth)
		newValue = MaxHealth;*/
	Health = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
	TextHP->SetText(FText::FromString(FString::SanitizeFloat(Health,0) + "/" + FString::SanitizeFloat(MaxHealth,0) + "HP"));
}

void UHealthWidget::OnPlayerMaxHealthChanged(float newValue)
{
	/*if (newValue < Health)
		Health = newValue;*/
	MaxHealth = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
	TextHP->SetText(FText::FromString(FString::SanitizeFloat(Health, 0) + "/" + FString::SanitizeFloat(MaxHealth, 0) + "HP"));
}