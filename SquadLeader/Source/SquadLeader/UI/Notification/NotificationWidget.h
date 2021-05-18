// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../SL_UserWidget.h"
#include "../Interface/NotificationInterface.h"

#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "NotificationWidgetElement.h"

#include "NotificationWidget.generated.h"

/**
 * 
 */
USTRUCT()
struct FNotificationWidgetSlot
{
	GENERATED_BODY()

	UNotificationWidgetElement* Widget;
	UVerticalBoxSlot* Slot;
};

UCLASS()
class SQUADLEADER_API UNotificationWidget : public USL_UserWidget, public INotificationInterface
{
	GENERATED_BODY()

public:
	UNotificationWidget(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY()
	TArray<FNotificationWidgetSlot> Items;

	UPROPERTY(EditDefaultsOnly, Category = "Notification")
	TSubclassOf<class UNotificationWidgetElementText> TextElementClass;

	UPROPERTY(EditDefaultsOnly, Category="Notification")
	int MaxItem = 10;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* MainPanel;

	void AddNotification(class UNotificationWidgetElement* newWidget);

	void UpdateNotificationsPosition();

	//-----USL_UserWidget-----
public:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn);

	//-----INotificationInterface-----
	void OnTextNotification_Received(FString textNotification) override;
};
