// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "SL_UserWidget.h"
#include "HealthWidget.h"
#include "ShieldWidget.h"
#include "AIInfoListWidget.h"
#include "Blueprint/UserWidget.h"
#include "../AI/AISquadController.h"
#include "../Soldiers/AIs/SoldierAI.h"

APlayerHUD::APlayerHUD()
{
}

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();
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
	//-----AIInfo-----
	if (AIInfoWidgetClass != nullptr)
	{
		AIInfoWidget = CreateWidget<UAIInfoListWidget>(GetWorld(), AIInfoWidgetClass);
		if (AIInfoWidget) {
			AIInfoWidget->AddToViewport();
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

void APlayerHUD::OnSquadChanged(TArray<AAISquadController*> newValue)
{
	AIInfoWidget->RemoveFromViewport();

	AIInfoWidget = CreateWidget<UAIInfoListWidget>(GetWorld(), AIInfoWidgetClass);
	if (AIInfoWidget) {
		AIInfoWidget->AddToViewport();
		for (auto AIController : newValue)
		{
			ASoldierAI* PlayerAI = Cast<ASoldierAI>(AIController->GetPawn());
			if (PlayerAI)
			{
				AIInfoWidget->AddItem(PlayerAI);
			}
		}
	}
}
