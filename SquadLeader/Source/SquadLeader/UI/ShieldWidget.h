// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Engine/Canvas.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ShieldWidget.generated.h"
/**
 *
 */
UCLASS()
class SQUADLEADER_API UShieldWidget : public USL_UserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UProgressBar* ProgressBarShield;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* TextShield;

	float Shield = 100;
	float MaxShield = 100;

public:
	UShieldWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void OnShieldChanged(float newValue);

	UFUNCTION(BlueprintCallable)
	void OnMaxShieldChanged(float newValue);
};