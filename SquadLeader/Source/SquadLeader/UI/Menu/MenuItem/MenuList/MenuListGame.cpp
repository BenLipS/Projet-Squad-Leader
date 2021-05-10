// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuListGame.h"

#include "Blueprint/WidgetTree.h"
#include "MenuListGameItemWidget.h"
#include "MenuListInfoItemWidget.h"

#include "../../MenuWidget.h"
#include "../../MenuLayoutWidget.h"

void UMenuListGame::OnItemAddedToLayout_Implementation()
{
	if (MenuLayout)
	{
		if (auto Menu = MenuLayout->GetMenu(); Menu)
		{
			if (auto MenuStat = Cast<IMenuGameInfoDelegateInterface>(Menu); MenuStat)
			{
				MenuStat->AddMenuGameInfoDelegate(this);
			}
		}
	}
}

void UMenuListGame::OnGamesInfoCleanOrder()
{
	ListEntry->ClearChildren();
}

void UMenuListGame::OnGameInfoReceived(FString Key, FString Value, FString IP)
{
	if (ItemClass)
	{
		auto newItem = WidgetTree->ConstructWidget<UMenuListGameItemWidget>(ItemClass);

		if (IsValid(newItem))
		{
			newItem->SetLeftData(Key);
			newItem->SetRightData(Value);
			newItem->SetIPAdress(IP);
			AddEntryToList(newItem);
		}
	}
}