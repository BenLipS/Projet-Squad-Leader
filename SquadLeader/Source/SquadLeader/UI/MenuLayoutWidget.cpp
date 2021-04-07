// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuLayoutWidget.h"

#include "MenuWidget.h"
#include "MenuItemWidget.h"

#include "Components/PanelWidget.h"
#include "Components/Widget.h"

UMenuWidget* UMenuLayoutWidget::GetMenu()
{
	return Menu;
}

void UMenuLayoutWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MenuSlot)
	{
		SynchronizeAllMenuItem(MenuSlot->GetAllChildren());
	}
}

void UMenuLayoutWidget::SynchronizeAllMenuItem(TArray<UWidget*> WidgetList)
{
	for (UWidget* Widget : WidgetList)
	{
		if (UMenuItemWidget* MenuItem = Cast<UMenuItemWidget>(Widget); MenuItem)
		{
			MenuItems.AddUnique(MenuItem);
			MenuItem->MenuLayout = this;
		}
		else if (UPanelWidget* MenuGroup = Cast<UPanelWidget>(Widget); MenuGroup)
		{
			SynchronizeAllMenuItem(MenuGroup->GetAllChildren());
		}
	}
}

FString UMenuLayoutWidget::GetLayoutID()
{
	return GetName();
}
