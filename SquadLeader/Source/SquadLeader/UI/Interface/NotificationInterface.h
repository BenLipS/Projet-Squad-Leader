// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NotificationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNotificationInterface : public UInterface
{
	GENERATED_BODY()
};

class SQUADLEADER_API INotificationInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnTextNotification_Received(FString textNotification) = 0;
};
