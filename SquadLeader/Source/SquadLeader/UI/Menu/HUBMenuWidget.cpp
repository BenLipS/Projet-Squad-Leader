// Fill out your copyright notice in the Description page of Project Settings.


#include "HUBMenuWidget.h"

void UHUBMenuWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (auto HUD = Cast<IMenuGameInfoDelegateInterface>(ObjectIn); HUD)
	{
		HUD->AddMenuGameInfoDelegate(this);
	}
}