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

bool UMenuItemWidget::DeselectItem()
{
	if (bIsSelected && IsValid(MenuLayout))
	{
		MenuLayout->DeselectItem(this);
		return true;
	}
	return false;
}

bool UMenuItemWidget::IsSelected()
{
	return bIsSelected;;
}

void UMenuItemWidget::OnItemSelected()
{
	OnItemSelectedEvent.Broadcast();
	BlueprintOnItemSelected();
	bIsSelected = true;
}

void UMenuItemWidget::OnItemDeselected()
{
	bIsSelected = false;
}

UMenuLayoutWidget* UMenuItemWidget::GetMenuLayout()
{
	return MenuLayout;
}

void UMenuItemWidget::OnItemAddedToLayout_Implementation()
{
}