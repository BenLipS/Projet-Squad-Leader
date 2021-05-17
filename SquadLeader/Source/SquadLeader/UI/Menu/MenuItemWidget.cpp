// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuItemWidget.h"
#include "MenuLayoutWidget.h"

void UMenuItemWidget::SelectItem()
{
	if (bIsSelectGlobal)
	{
		MenuLayout->OnItemSelected(this);
	}
}

void UMenuItemWidget::OnItemSelected()
{
	OnItemSelectedEvent.Broadcast();
	BlueprintOnItemSelected();
}

void UMenuItemWidget::OnItemDeselected()
{
}

UMenuLayoutWidget* UMenuItemWidget::GetMenuLayout()
{
	return MenuLayout;
}

void UMenuItemWidget::OnItemAddedToLayout_Implementation()
{
}