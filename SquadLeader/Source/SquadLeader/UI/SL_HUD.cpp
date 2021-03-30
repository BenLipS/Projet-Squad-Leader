// Fill out your copyright notice in the Description page of Project Settings.


#include "SL_HUD.h"
#include "SL_UserWidget.h"

#include "../Soldiers/Players/SoldierPlayerController.h"
#include "../Soldiers/Players/SoldierPlayerState.h"

ASL_HUD::ASL_HUD()
{
}

void ASL_HUD::BeginPlay()
{
	Super::BeginPlay();
	for (TSubclassOf<USL_UserWidget> WidgetClass : WidgetClassList)
	{
		if (WidgetClass)
		{
			USL_UserWidget* newEntry = CreateWidget<USL_UserWidget>(GetWorld(), WidgetClass);
			if (newEntry)
			{
				newEntry->AddToViewport();
				WidgetList.Add(newEntry);
				newEntry->SetupDelegateToObject(this);
			}
		}
	}
	SetPlayerStateLink();
	SetAIStateLink();
}

void ASL_HUD::SetPlayerStateLink()
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

			PS->OnHealthChanged.AddDynamic(this, &ASL_HUD::OnPlayerHealthChanged);
			PS->OnMaxHealthChanged.AddDynamic(this, &ASL_HUD::OnPlayerMaxHealthChanged);
			PS->OnShieldChanged.AddDynamic(this, &ASL_HUD::OnPlayerShieldChanged);
			PS->OnMaxShieldChanged.AddDynamic(this, &ASL_HUD::OnPlayerMaxShieldChanged);

			PS->BroadCastAllDatas();
		}
	}
}

void ASL_HUD::SetAIStateLink()
{
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwningPlayerController()); PC)
	{
		PC->BroadCastManagerData();
	}
}