#include "RespawnMenu.h"

#include "MenuLayoutWidget.h"
#include "RespawnMenuLayout.h"
#include "MenuItem/MenuButtonWidget.h"
#include "MenuItem/MenuButtonControlAreaWidget.h"

void URespawnMenu::SynchronizeProperties()
{
	UMenuWidget::SynchronizeProperties();
}

void URespawnMenu::SetupDelegateToObject_Implementation(UObject* _ObjectIn)
{
	if (IMinimapDelegateInterface* MinimapDelegateInterface = Cast<IMinimapDelegateInterface>(_ObjectIn); MinimapDelegateInterface)
	{
		MinimapDelegateInterface->AddMinimapDelegate(this);
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

