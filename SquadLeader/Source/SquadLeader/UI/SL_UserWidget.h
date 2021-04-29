// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SL_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API USL_UserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SL_UserWidget")
	int32 DesiredZOrderInViewport = 0;

public:
	int32 GetDesiredZOrderInViewport();

public:
	USL_UserWidget(const FObjectInitializer& ObjectInitializer);

	virtual void AddToViewport();

	/*Call by HUD to setup the delegate interface*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetupDelegateToObject(UObject* ObjectIn);
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn);
};