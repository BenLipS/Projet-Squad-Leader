// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationWidgetElementText.h"

void UNotificationWidgetElementText::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UNotificationWidgetElement::NativeOnListItemObjectSet(ListItemObject);
}

void UNotificationWidgetElementText::InitText(FText newText)
{
	if(TextNotification)
		TextNotification->SetText(FText::FromString("fonctionnel"));
}
