// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCollectionDataInt.h"

#include "Components/Slider.h"

#include "SliderCollectionDataItem.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API USliderCollectionDataItem : public UMenuCollectionDataInt
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USlider* SliderData;

protected:
	virtual void SynchronizeProperties() override;

public:
	virtual void SetDataValue(int newValue) override;
	virtual void SetMinValue(int newValue) override;
	virtual void SetMaxValue(int newValue) override;

protected:
	UFUNCTION()
	void OnSliderValueChange(float newValue);
};
