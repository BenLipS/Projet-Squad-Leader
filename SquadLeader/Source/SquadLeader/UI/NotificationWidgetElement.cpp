// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationWidgetElement.h"

void UNotificationWidgetElement::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}
