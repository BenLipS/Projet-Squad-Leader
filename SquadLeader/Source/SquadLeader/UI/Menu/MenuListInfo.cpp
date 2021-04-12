// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuListInfo.h"

#include "Blueprint/WidgetTree.h"
#include "MenuListInfoItemWidget.h"
#include "../Interface/StatInfoDelegateInterface.h"
#include "MenuLayoutWidget.h"
#include "MenuWidget.h"

void UMenuListInfo::AddEntryToList(UMenuListInfoItemWidget* Entry)
{
	ListEntry->AddChild(Entry);
}

void UMenuListInfo::OnItemAddedToLayout_Implementation()
{
	if (MenuLayout)
	{
		if (UMenuWidget* Menu = MenuLayout->GetMenu(); Menu)
		{
			if (IStatInfoDelegateInterface* MenuStat = Cast<IStatInfoDelegateInterface>(Menu); MenuStat)
			{
				MenuStat->AddStatInfoDelegate(this);
			}
		}
	}
}

void UMenuListInfo::OnStatInfoReceived(FString Key, FString Value)
{
	if (ItemClass)
	{
		UMenuListInfoItemWidget* newItem = WidgetTree->ConstructWidget<UMenuListInfoItemWidget>(ItemClass);
		newItem->SetRightData(Key);
		newItem->SetLeftData(Value);
		AddEntryToList(newItem);
	}
}

void UMenuListInfo::OnStatsInfoReceived(TMap<FString, FString> statsIn)
{
	for (auto StatIn : statsIn)
	{
		OnStatInfoReceived(StatIn.Key, StatIn.Value);
	}
}
