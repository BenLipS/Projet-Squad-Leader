// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuListInfo.h"

#include "Blueprint/WidgetTree.h"
#include "MenuListInfoItemWidget.h"
#include "../../../Interface/StatInfoDelegateInterface.h"
#include "../../MenuLayoutWidget.h"
#include "../../MenuWidget.h"
#include "MenuListInfoItemWidget.h"

void UMenuListInfo::AddEntryToList(UMenuListInfoItemWidget* Entry)
{
	ListEntry->AddChild(Entry);
	Entry->OwnerList = this;
}

UMenuListInfoItemWidget* UMenuListInfo::GetSelectedItem()
{
	return ItemSelected;
}