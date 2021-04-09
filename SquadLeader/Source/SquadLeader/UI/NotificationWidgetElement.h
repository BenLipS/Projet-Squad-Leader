// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "NotificationWidgetElement.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UNotificationWidgetElement : public USL_UserWidget
{
	GENERATED_BODY()

protected:
	class UNotificationWidget* NotificationWidget;
	
public:
	UFUNCTION(BlueprintCallable)
	UNotificationWidget* GetNotificationWidget();

	void BindItemToNotification(class UNotificationWidget* newNotificationWidget);

protected:
	/*Called when added to a notification list*/
	UFUNCTION(BlueprintNativeEvent, Category = "NotificationWidgetElement")
	void OnItemAddedToNotificationEvent(class UNotificationWidget* newNotificationWidget);
	virtual void OnItemAddedToNotificationEvent_Implementation(class UNotificationWidget* newNotificationWidget);
};