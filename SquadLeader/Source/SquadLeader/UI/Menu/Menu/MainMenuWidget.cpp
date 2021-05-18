// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UMainMenuWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (auto HUDStatInfo = Cast<IStatInfoDelegateInterface>(ObjectIn); HUDStatInfo)
	{
		HUDStatInfo->AddStatInfoDelegate(this);
	}
	if (auto HUD = Cast<IMenuGameInfoDelegateInterface>(ObjectIn); HUD)
	{
		HUD->AddMenuGameInfoDelegate(this);
	}
}

void UMainMenuWidget::SynchronizeProperties()
{
	UMenuWidget::SynchronizeProperties();

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