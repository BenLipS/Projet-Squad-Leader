// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCollectionDataTextBoxInt.h"

#include "Types/SlateEnums.h"

void UMenuCollectionDataTextBoxInt::SynchronizeProperties()
{
	UMenuCollectionDataInt::SynchronizeProperties();

	if (IsValid(TextBoxData))
	{
		SetMinValue(MinValue);
		SetMaxValue(MaxValue);
		SetDataValue(DataValue);
	}
}

bool UMenuCollectionDataTextBoxInt::SetDataValue(int newValue, bool fireEvent)
{
	if (UMenuCollectionDataInt::SetDataValue(newValue, fireEvent))
	{
		if (IsValid(TextBoxData))
		{
			TextBoxData->SetText(FText::FromString(FString::FromInt(DataValue)));
		}
		return true;
	}
	return false;
}

bool UMenuCollectionDataTextBoxInt::SetMinValue(int newValue)
{
	return UMenuCollectionDataInt::SetMinValue(newValue);
}

bool UMenuCollectionDataTextBoxInt::SetMaxValue(int newValue)
{
	return UMenuCollectionDataInt::SetMaxValue(newValue);
}
