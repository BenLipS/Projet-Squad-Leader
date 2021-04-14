// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuItemWidget.h"
#include "MenuLayoutWidget.h"

UMenuLayoutWidget* UMenuItemWidget::GetMenuLayout()
{
	return MenuLayout;
}

void UMenuItemWidget::OnItemAddedToLayout_Implementation()
{
}
