// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Interface/NotificationInterface.h"

#include "Components/TileView.h"

#include "NotificationWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UNotificationWidget : public USL_UserWidget, public INotificationInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTileView* TileViewNotification;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UNotificationWidgetElementText> TextElementClass;

	//-----USL_UserWidget-----
public:
	virtual void SetupDelegateToObject(UObject* ObjectIn);
	//-----USL_UserWidget-----

	UFUNCTION()
	void OnTextNotification_Received(FText textNotification) override;
};
