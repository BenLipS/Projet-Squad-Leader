// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "SL_UserWidget.h"
#include "HealthWidget.h"
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
	if (WidgetHealthClass != nullptr)
	{
		WidgetHealth = CreateWidget<UHealthWidget>(GetWorld(), WidgetHealthClass);
		if (WidgetHealth) {
			WidgetHealth->AddToViewport();
		}
	}
}

void APlayerHUD::OnHealthChanged(float newValue)
{
	if (WidgetHealth)
	{
		WidgetHealth->OnHealthChanged(newValue);
	}
}

void APlayerHUD::OnMaxHealthChanged(float newValue)
{
	if (WidgetHealth)
	{
		WidgetHealth->OnMaxHealthChanged(newValue);
	}
}
