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

public:
	USL_UserWidget(const FObjectInitializer& ObjectInitializer);

	/*Call by HUD to setup the delegate interface*/
	virtual void SetupDelegateToObject(UObject* ObjectIn);
};
