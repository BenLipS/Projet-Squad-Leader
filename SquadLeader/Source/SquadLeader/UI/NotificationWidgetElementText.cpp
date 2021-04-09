// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationWidgetElementText.h"

void UNotificationWidgetElementText::InitText(FString newText)
{
	if(TextNotification)
		TextNotification->SetText(FText::FromString(newText));
}
