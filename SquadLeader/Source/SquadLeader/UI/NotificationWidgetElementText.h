// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotificationWidgetElement.h"

#include "Components/TextBlock.h"

#include "NotificationWidgetElementText.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UNotificationWidgetElementText : public UNotificationWidgetElement
{
	GENERATED_BODY()

	//-----IUserObjectListEntry-----
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextNotification;

public:
	UFUNCTION(BlueprintCallable)
	void InitText(FText newText);
};