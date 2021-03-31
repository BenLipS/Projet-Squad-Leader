// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationDelegateInterface.h"

// Add default functionality here for any INotificationDelegateInterface functions that are not pure virtual.

void INotificationDelegateInterface::OnTextNotification_Received(FText textNotification)
{
	for (auto NotificationInterface : NotificationDelegates)
	{
		NotificationInterface->OnTextNotification_Received(textNotification);
	}
}

void INotificationDelegateInterface::AddNotificationDelegate(INotificationInterface* newDelegate)
{
	NotificationDelegates.AddUnique(newDelegate);
}

void INotificationDelegateInterface::RemoveNotificationDelegate(INotificationInterface* removeDelegate)
{
	NotificationDelegates.Remove(removeDelegate);
}
