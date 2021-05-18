// Fill out your copyright notice in the Description page of Project Settings.


#include "HUBMenuWidget.h"

#include "SquadLeader/MainMenu/GameParam/GameParam.h"
#include "SquadLeader/SquadLeaderGameInstance.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UHUBMenuWidget::InitFromGameparam(TSubclassOf<AGameParam> gameParam)
{
	if (IsValid(ClassSelectionLayout) && IsValid(gameParam))
	{
		for (int i = 0; i < gameParam.GetDefaultObject()->StartingNbAISquad + 2; i++)
		{
			ClassSelectionLayout->AddClassEntryToClassBox();
		}
	}
}

void UHUBMenuWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
}

void UHUBMenuWidget::SynchronizeProperties()
{
	UMenuWidget::SynchronizeProperties();
	if (auto GI = GetGameInstance<USquadLeaderGameInstance>(); GI && IsValid(GI->GameParam))
	{
		InitFromGameparam(GI->GameParam);
	}
	if (auto PC = GetOwningPlayer(); PC)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		InputMode.SetWidgetToFocus(TakeWidget());

		PC->SetInputMode(InputMode);

		PC->SetShowMouseCursor(true);
		FVector2D Center = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / 2.0f;
		PC->SetMouseLocation(Center.X, Center.Y);
	}
}