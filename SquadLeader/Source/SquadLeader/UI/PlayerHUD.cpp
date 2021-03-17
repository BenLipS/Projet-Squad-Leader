// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "SL_UserWidget.h"
#include "HealthWidget.h"
#include "ShieldWidget.h"
#include "AIInfoListWidget.h"
#include "AIInfoWidget.h"
#include "Blueprint/UserWidget.h"
#include "../AI/AISquadController.h"
#include "../Soldiers/Players/SoldierPlayerController.h"
#include "../Soldiers/Players/SoldierPlayerState.h"
#include "../Soldiers/Players/SoldierPlayer.h"

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
	//-----CrossHair-----
	/*if (CrosshairWidgetClass != nullptr)
	{
		CrosshairWidget = CreateWidget<USL_UserWidget>(GetWorld(), CrosshairWidgetClass);
		if (CrosshairWidget) {
			CrosshairWidget->AddToViewport();
		}
	}*/
	
	//-----AIInfo-----
	/*if (AIWidgetClass != nullptr)
	{
		AIWidget = CreateWidget<UAIInfoWidget>(GetWorld(), AIWidgetClass);
		if (AIWidget) {
			AIWidget->AddToViewport();
		}
	}*/
	if (AIListInfoWidgetClass != nullptr)
	{
		AIListInfoWidget = CreateWidget<UAIInfoListWidget>(GetWorld(), AIListInfoWidgetClass);
		if (AIListInfoWidget) {
			AIListInfoWidget->AddToViewport();
		}
	}
	SetPlayerStateLink();
	SetAIStateLink();
}

void APlayerHUD::SetPlayerStateLink()
{
	ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwningPlayerController());
	if (PC)
	{
		ASoldierPlayerState* PS = PC->GetPlayerState<ASoldierPlayerState>();
		if (PS)
		{
			//TODO faire ça plus propre ?
			PS->OnHealthChanged.RemoveAll(this);
			PS->OnMaxHealthChanged.RemoveAll(this);
			PS->OnShieldChanged.RemoveAll(this);
			PS->OnMaxShieldChanged.RemoveAll(this);

			PS->OnHealthChanged.AddDynamic(this, &APlayerHUD::OnHealthChanged);
			PS->OnMaxHealthChanged.AddDynamic(this, &APlayerHUD::OnMaxHealthChanged);
			PS->OnShieldChanged.AddDynamic(this, &APlayerHUD::OnShieldChanged);
			PS->OnMaxShieldChanged.AddDynamic(this, &APlayerHUD::OnMaxShieldChanged);
			PS->BroadCastAllDatas();
		}
	}
}

void APlayerHUD::SetAIStateLink()
{
	ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwningPlayerController());
	if (PC)
	{
		PC->BroadCastManagerData();
	}
}

void APlayerHUD::OnHealthChanged(float newValue)
{
	GetOwningPlayerController();
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

void APlayerHUD::OnSquadChanged(const TArray<FSoldierAIData>& newValue)
{
	if (AIListInfoWidget)
	{
		AIListInfoWidget->OnSquadChanged(newValue);
	}
}

void APlayerHUD::OnSquadHealthChanged(int index, float newHealth)
{
	AIListInfoWidget->OnSquadHealthChanged(index, newHealth);
}

void APlayerHUD::OnSquadMaxHealthChanged(int index, float newHealth)
{
	AIListInfoWidget->OnSquadMaxHealthChanged(index, newHealth);
}

void APlayerHUD::OnSquadShieldChanged(int index, float newShield)
{
	AIListInfoWidget->OnSquadShieldChanged(index, newShield);
}

void APlayerHUD::OnSquadMaxShieldChanged(int index, float newMaxShield)
{
	AIListInfoWidget->OnSquadMaxShieldChanged(index, newMaxShield);
}
