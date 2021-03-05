// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "SL_UserWidget.h"
#include "HealthWidget.h"
#include "ShieldWidget.h"
#include "Blueprint/UserWidget.h"

APlayerHUD::APlayerHUD()
{
}

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();
	/*WidgetHealth->RemoveFromViewport();
	WidgetHealth->AddToViewport();*/
}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	//Creating widget

	//-----Health-----
	if (HealthWidgetClass != nullptr)
	{
		HealthWidget = CreateWidget<UHealthWidget>(GetWorld(), HealthWidgetClass);
		if (HealthWidget) {
			HealthWidget->AddToViewport();
		}
	}
	//-----Shield-----
	if (ShieldWidgetClass != nullptr)
	{
		ShieldWidget = CreateWidget<UShieldWidget>(GetWorld(), ShieldWidgetClass);
		if (ShieldWidget) {
			ShieldWidget->AddToViewport();
		}
	}
}

void APlayerHUD::OnHealthChanged(float newValue)
{
	if (HealthWidget)
	{
		HealthWidget->OnHealthChanged(newValue);
	}
}

void APlayerHUD::OnMaxHealthChanged(float newValue)
{
	if (HealthWidget)
	{
		HealthWidget->OnMaxHealthChanged(newValue);
	}
}

void APlayerHUD::OnShieldChanged(float newValue)
{
	if (ShieldWidget)
	{
		ShieldWidget->OnShieldChanged(newValue);
	}
}

void APlayerHUD::OnMaxShieldChanged(float newValue)
{
	if (ShieldWidget)
	{
		ShieldWidget->OnMaxShieldChanged(newValue);
	}
}