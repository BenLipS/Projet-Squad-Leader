#include "RespawnMenu.h"

#include "MenuLayoutWidget.h"
#include "RespawnMenuLayout.h"
#include "MenuItem/MenuButtonWidget.h"
#include "MenuItem/MenuButtonControlAreaWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"

void URespawnMenu::SynchronizeProperties()
{
	UMenuWidget::SynchronizeProperties();
}

void URespawnMenu::OnRespawnScreenDeactivated()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (auto PC = GetOwningPlayer(); PC)
	{
		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(false);
		PC->SetInputMode(InputMode);

		PC->SetShowMouseCursor(false);
	}
}

void URespawnMenu::SetupDelegateToObject_Implementation(UObject* _ObjectIn)
{
	if (IMinimapDelegateInterface* MinimapDelegateInterface = Cast<IMinimapDelegateInterface>(_ObjectIn); MinimapDelegateInterface)
	{
		MinimapDelegateInterface->AddMinimapDelegate(this);
	}
	if (IRespawnInterface* RespawnInterface = Cast<IRespawnInterface>(_ObjectIn); RespawnInterface)
	{
		RespawnInterface->AddRespawnDelegate(this);
	}
}

void URespawnMenu::OnControlAreaAdded(AControlArea* _ControlArea)
{
	if (IsValid(RespawnButtonClass))
	{
		auto newWidget = Cast<UMenuButtonControlAreaWidget>(LayoutRespawn->AddChildToRespawnBox(RespawnButtonClass));
		newWidget->SetControlArealink(_ControlArea);

	}
}

void URespawnMenu::OnRespawnScreenActivated()
{
	SetVisibility(ESlateVisibility::Visible);
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

