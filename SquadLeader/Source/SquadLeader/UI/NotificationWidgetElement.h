// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "NotificationWidgetElement.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UNotificationWidgetElement : public USL_UserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);
};