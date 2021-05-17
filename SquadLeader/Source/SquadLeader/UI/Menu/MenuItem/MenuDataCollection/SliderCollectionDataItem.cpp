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

bool USliderCollectionDataItem::SetDataValue(int newValue, bool fireEvent)
{
	if (UMenuCollectionDataInt::SetDataValue(newValue, fireEvent))
	{
		if (IsValid(SliderData))
		{
			SliderData->SetValue(DataValue);
		}
		return true;
	}
	return false;
}

bool USliderCollectionDataItem::SetMinValue(int newValue)
{
	if (UMenuCollectionDataInt::SetMinValue(newValue))
	{
		if (IsValid(SliderData))
		{
			SliderData->SetMinValue(MinValue);
		}
		return true;
	}
	return false;
}

bool USliderCollectionDataItem::SetMaxValue(int newValue)
{
	if (UMenuCollectionDataInt::SetMaxValue(newValue))
	{
		if (IsValid(SliderData))
		{
			SliderData->SetMaxValue(MaxValue);
		}
		return true;
	}
	return false;
}

void USliderCollectionDataItem::OnSliderValueChange(float newValue)
{
	SetDataValue(FMath::RoundToInt(newValue));
}