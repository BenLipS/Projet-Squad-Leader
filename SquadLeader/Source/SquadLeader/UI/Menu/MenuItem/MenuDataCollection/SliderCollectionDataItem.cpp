// Fill out your copyright notice in the Description page of Project Settings.


#include "SliderCollectionDataItem.h"

void USliderCollectionDataItem::SynchronizeProperties()
{
	UMenuCollectionDataInt::SynchronizeProperties();

	if (IsValid(SliderData))
	{
		SetMinValue(MinValue);
		SetMaxValue(MaxValue);
		SetDataValue(DataValue);
		SliderData->SetStepSize(1.f);
		SliderData->MouseUsesStep = true;
		SliderData->OnValueChanged.AddDynamic(this, &USliderCollectionDataItem::OnSliderValueChange);
	}
}

void USliderCollectionDataItem::SetDataValue(int newValue)
{
	UMenuCollectionDataInt::SetDataValue(newValue);

	if (IsValid(SliderData))
	{
		SliderData->SetValue(DataValue);
	}
}

void USliderCollectionDataItem::SetMinValue(int newValue)
{
	UMenuCollectionDataInt::SetMinValue(newValue);

	if (IsValid(SliderData))
	{
		SliderData->SetMinValue(MinValue);
	}
}

void USliderCollectionDataItem::SetMaxValue(int newValue)
{
	UMenuCollectionDataInt::SetMaxValue(newValue);

	if (IsValid(SliderData))
	{
		SliderData->SetMaxValue(MaxValue);
	}
}

void USliderCollectionDataItem::OnSliderValueChange(float newValue)
{
	SetDataValue(FMath::RoundToInt(newValue));
}