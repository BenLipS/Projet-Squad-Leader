// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoWidget.h"

UAmmoWidget::UAmmoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAmmoWidget::OnAmmoChanged(uint8 newValue)
{
	Ammo = newValue;
	ProgressBarAmmo->SetPercent(float(Ammo) / MaxAmmo);
	TextAmmo->SetText(FText::FromString(FString::FromInt(Ammo)));
}

void UAmmoWidget::OnMaxAmmoChanged(uint8 newValue)
{
	MaxAmmo = newValue;
	ProgressBarAmmo->SetPercent(float(Ammo) / MaxAmmo);
}
