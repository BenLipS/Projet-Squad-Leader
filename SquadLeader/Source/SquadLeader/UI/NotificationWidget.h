// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Interface/NotificationInterface.h"

#include "Components/CanvasPanel.h"

#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "NotificationWidget.generated.h"

/**
 * 
 */
USTRUCT()
struct FTextBlockSlot
{
	GENERATED_BODY()

	UTextBlock* TextBlock;
	UCanvasPanelSlot* Slot;
};

UCLASS()
class SQUADLEADER_API UNotificationWidget : public USL_UserWidget, public INotificationInterface
{
	GENERATED_BODY()

public:
	UNotificationWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TArray<FTextBlockSlot> Items;

	UPROPERTY(EditDefaultsOnly, Category="Notification")
	int MaxItem = 10;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* MainPanel;

	UFUNCTION()
	void AddNotification(FString textNotification);

	void UpdateNotificationsPosition();

	//-----USL_UserWidget-----
public:
	virtual void SetupDelegateToObject(UObject* ObjectIn);

	//-----INotificationInterface-----
	void OnTextNotification_Received(FString textNotification) override;
};
