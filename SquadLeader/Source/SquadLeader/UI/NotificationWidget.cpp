// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationWidget.h"

#include "Interface/NotificationDelegateInterface.h"

#include "NotificationWidgetElement.h"
#include "NotificationWidgetElementText.h"

void UNotificationWidget::SetupDelegateToObject(UObject* ObjectIn)
{
	if (INotificationDelegateInterface* NotificationDelegateInterface = Cast<INotificationDelegateInterface>(ObjectIn); NotificationDelegateInterface)
	{
		NotificationDelegateInterface->AddNotificationDelegate(this);
	}
}

void UNotificationWidget::OnTextNotification_Received(FText textNotification)
{
	UNotificationWidgetElementText* newItem = CreateWidget<UNotificationWidgetElementText>(GetWorld(), TextElementClass);
	newItem->InitText(textNotification);
	TileViewNotification->AddItem(newItem);
}