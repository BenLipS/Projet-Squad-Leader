// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Components/Image.h"
#include "Minimap/PointOfInterestWidget.h"
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

public:
	UFUNCTION()
	void OnControlAreaCapturerChange(int newOwner);
	UFUNCTION()
	void OnControlAreaOwnerChange(int newCapturer);
	UFUNCTION()
	void OnControlAreaPercentageChange(float Percentage);

private:
	UFUNCTION()
	FLinearColor GetColorFromOwner(int ownerIn);
};
