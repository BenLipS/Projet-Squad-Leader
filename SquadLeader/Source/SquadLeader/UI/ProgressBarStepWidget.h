// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"

#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Layout/Margin.h"

#include "ProgressBarStepWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UProgressBarStepWidget : public USL_UserWidget
{
	GENERATED_BODY()

protected:
	virtual void SynchronizeProperties() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UProgressBar> ProgressBarModel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* ContainerBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = "0", UIMax = "1"), Category = "ProgressStep")
	float Percent = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = "1"), Category = "ProgressStep")
	float NbSplit = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProgressStep | Slot")
	float LeftPadding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProgressStep | Slot")
	float TopPadding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProgressStep | Slot")
	float RightPadding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProgressStep | Slot")
	float BottomPadding;

public:
	UFUNCTION(BlueprintCallable, Category = "ProgressStep")
	void SetPercent(float InPercent);

	UFUNCTION(BlueprintCallable, Category = "ProgressStep")
	void SetNbSplit(float InNbSplit);

protected:
	UFUNCTION()
	void UpdatePercentDisplay();

	UFUNCTION()
	void UpdateNbSplit();
};
