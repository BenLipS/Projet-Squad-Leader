// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Minimap/PointOfInterestWidget.h"
#include "Animation/WidgetAnimation.h"

#include "ControlAreaInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UControlAreaInfoWidget : public UPointOfInterestWidget
{

	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor AllyColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor EnnemyColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor NeutralColor;

	UPROPERTY(BlueprintReadOnly)
	int Owner;
	
	UPROPERTY(BlueprintReadOnly)
	int Capturer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ImageControlArea;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ControlAreaName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* CaptureAnimation;

public:
	UFUNCTION()
	void OnControlAreaCapturerChange(int newOwner);
	UFUNCTION()
	void OnControlAreaOwnerChange(int newCapturer);
	UFUNCTION()
	void OnControlAreaPercentageChange(float Percentage);
	
	UFUNCTION()
	void OnControlAreaNameChange(FString NameIn);

private:
	UFUNCTION()
	FLinearColor GetColorFromOwner(int ownerIn);
};
