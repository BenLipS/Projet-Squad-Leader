// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationWidget.h"
#include "NotificationWidgetElement.h"

UNotificationWidget* UNotificationWidgetElement::GetNotificationWidget()
{
	return NotificationWidget;
}

void UNotificationWidgetElement::BindItemToNotification(UNotificationWidget* newNotificationWidget)
{
	NotificationWidget = newNotificationWidget;
	OnItemAddedToNotificationEvent(newNotificationWidget);
}

void UNotificationWidgetElement::OnItemAddedToNotificationEvent_Implementation(class UNotificationWidget* newNotificationWidget)
{

}