// Fill out your copyright notice in the Description page of Project Settings.


#include "GameEndWidget.h"
#include "Interface/GameEndDelegateInterface.h"

void UGameEndWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (IGameEndDelegateInterface* GameEndDelegateInterface = Cast<IGameEndDelegateInterface>(ObjectIn); GameEndDelegateInterface)
	{
		GameEndDelegateInterface->AddGameEndDelegate(this);
	}
}

void UGameEndWidget::OnGameEnd(FString EndMessage)
{
	TextEndGame->SetText(FText::FromString(EndMessage));
	SetVisibility(ESlateVisibility::Visible);
}

void UGameEndWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (!IsDesignTime())
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
