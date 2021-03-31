// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NotificationInterface.h"
#include "NotificationDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNotificationDelegateInterface : public UNotificationInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API INotificationDelegateInterface : public INotificationInterface
{
	GENERATED_BODY()
private:
	TArray<INotificationInterface*> NotificationDelegates;

public:
	virtual void OnTextNotification_Received(FText textNotification) override;

	void AddNotificationDelegate(INotificationInterface* newDelegate);
	void RemoveNotificationDelegate(INotificationInterface* removeDelegate);
};
