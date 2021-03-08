// Fill out your copyright notice in the Description page of Project Settings.

#include "AIInfoListWidget.h"


UAIInfoListWidget::UAIInfoListWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAIInfoListWidget::AddItem(float Health, float HealthMax)
{
	if (AIInfoWidgetClass != nullptr)
	{
		UAIInfoWidget* entry = CreateWidget<UAIInfoWidget>(GetWorld(), AIInfoWidgetClass);
		if (entry) {
			ListWidget.Add(entry);
			AIInfoTileView->AddItem(entry);
			/*entry->OnHealthChanged(Health);
			entry->OnMaxHealthChanged(HealthMax);*/
			//entry->AddToViewport();
		}
	}
}
