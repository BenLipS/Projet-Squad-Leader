// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCollectionDataInt.h"
#include "SliderCollectionDataItem.h"
#include "MenuCollectionDataTextBoxInt.h"

#include "Components/TextBlock.h"

#include "MenuCollectionDataSliderTextBox.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuCollectionDataSliderTextBox : public UMenuCollectionDataInt
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UMenuCollectionDataTextBoxInt* TextBoxInt;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USliderCollectionDataItem* SliderInt;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString TextWidget;

protected:
	virtual void SynchronizeProperties() override;

public:
	virtual bool SetDataValue(int newValue, bool fireEvent = true) override;
	virtual bool SetMinValue(int newValue) override;
	virtual bool SetMaxValue(int newValue) override;

	UFUNCTION(BlueprintCallable)
	void SetText(const FString& TextWidgetIn);

	UFUNCTION(BlueprintCallable)
	FString GetText();
};
