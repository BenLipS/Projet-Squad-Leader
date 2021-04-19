// Fill out your copyright notice in the Description page of Project Settings.

#include "AIInfoListWidget.h"
#include "AIInfoWidget.h"
#include "../Soldiers/AIs/SoldierAI.h"
#include "Blueprint/WidgetTree.h"
#include "Interface/SquadDelegateInterface.h"


UAIInfoListWidget::UAIInfoListWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), AIInfoWidgetList()
{
}

void UAIInfoListWidget::OnSquadChanged(const TArray<FSoldierAIData>& newValue)
{
	AIInfoWidgetList.Empty();
	AIInfoContainer->ClearChildren();
	for (auto Soldier : newValue)
	{
		UAIInfoWidget* newEntry = WidgetTree->ConstructWidget<UAIInfoWidget>(AIInfoWidgetClass);
		newEntry->OnHealthChanged(Soldier.Health);
		newEntry->OnMaxHealthChanged(Soldier.MaxHealth);
		newEntry->OnShieldChanged(Soldier.Shield);
		newEntry->OnMaxShieldChanged(Soldier.MaxShield);
		AIInfoWidgetList.Add(newEntry);
		AIInfoContainer->AddChildToVerticalBox(newEntry);
	}
}

void UAIInfoListWidget::OnSquadHealthChanged(int index, float newHealth)
{
	if (AIInfoWidgetList.IsValidIndex(index))
	{
		AIInfoWidgetList[index]->OnHealthChanged(newHealth);
	}
}

void UAIInfoListWidget::OnSquadMaxHealthChanged(int index, float newHealth)
{
	if (AIInfoWidgetList.IsValidIndex(index))
	{
		AIInfoWidgetList[index]->OnMaxHealthChanged(newHealth);
	}
}

void UAIInfoListWidget::OnSquadShieldChanged(int index, float newShield)
{
	if (AIInfoWidgetList.IsValidIndex(index))
	{
		AIInfoWidgetList[index]->OnShieldChanged(newShield);
	}
}

void UAIInfoListWidget::OnSquadMaxShieldChanged(int index, float newMaxShield)
{
	if (AIInfoWidgetList.IsValidIndex(index))
	{
		AIInfoWidgetList[index]->OnMaxShieldChanged(newMaxShield);
	}
}

void UAIInfoListWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (ISquadDelegateInterface* SquadDelegateInterface = Cast<ISquadDelegateInterface>(ObjectIn); SquadDelegateInterface)
	{
		SquadDelegateInterface->AddSquadDelegate(this);
	}
}