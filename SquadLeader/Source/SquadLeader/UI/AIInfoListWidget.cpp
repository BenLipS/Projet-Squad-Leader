// Fill out your copyright notice in the Description page of Project Settings.

#include "AIInfoListWidget.h"
#include "../Soldiers/AIs/SoldierAI.h"


UAIInfoListWidget::UAIInfoListWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAIInfoListWidget::AddItem(ASoldierAI* SoldierBound)
{
	/*if (AIInfoWidgetClass != nullptr)
	{
		UAIInfoWidget* entry = CreateWidget<UAIInfoWidget>(GetWorld(), AIInfoWidgetClass);
		if (entry) {
			SoldierBound->OnHealthChanged.AddDynamic(entry, &UAIInfoWidget::OnHealthChanged);
			SoldierBound->OnMaxHealthChanged.AddDynamic(entry, &UAIInfoWidget::OnMaxHealthChanged);
			SoldierBound->BroadCastDatas();
			entry->AddToViewport();
			//ListWidget.Add(entry);
			AIInfoTileView->AddItem(entry);
		}
	}*/
}