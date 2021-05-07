// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

void UMainMenuWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (auto HUDStatInfo = Cast<IStatInfoDelegateInterface>(ObjectIn); HUDStatInfo)
	{
		HUDStatInfo->AddStatInfoDelegate(this);
	}
}