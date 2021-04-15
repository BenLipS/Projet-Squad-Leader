// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuListInfoStat.h"

#include "Blueprint/WidgetTree.h"
#include "MenuListInfoItemWidget.h"
#include "../Interface/StatInfoDelegateInterface.h"
#include "MenuLayoutWidget.h"
#include "MenuWidget.h"

//void UMenuListInfoStat::OnItemAddedToLayout_Implementation()
//{
//	if (MenuLayout)
//	{
//		if (UMenuWidget* Menu = MenuLayout->GetMenu(); Menu)
//		{
//			if (IStatInfoDelegateInterface* MenuStat = Cast<IStatInfoDelegateInterface>(Menu); MenuStat)
//			{
//				MenuStat->AddStatInfoDelegate(this);
//			}
//		}
//	}
//}
//
//void UMenuListInfoStat::OnStatInfoReceived(FString Key, FString Value)
//{
//	WidgetTree->ConstructWidget<>()
//}
//
//void UMenuListInfoStat::OnStatsInfoReceived(TMap<FString, FString> statsIn)
//{
//}
