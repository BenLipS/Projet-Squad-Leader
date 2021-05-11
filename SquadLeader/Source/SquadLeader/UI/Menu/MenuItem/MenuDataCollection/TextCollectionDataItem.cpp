// Fill out your copyright notice in the Description page of Project Settings.


#include "TextCollectionDataItem.h"

#include "Types/SlateEnums.h"

void UTextCollectionDataItem::SynchronizeProperties()
{
	UMenuCollectionDataInt::SynchronizeProperties();

	if (IsValid(TextBoxData))
	{
		SetMinValue(MinValue);
		SetMaxValue(MaxValue);
		SetDataValue(DataValue);
	}
}

void UTextCollectionDataItem::SetDataValue(int newValue)
{
	UMenuCollectionDataInt::SetDataValue(newValue);

	if (IsValid(TextBoxData))
	{
		TextBoxData->SetText(FText::FromString(FString::FromInt(DataValue)));
	}
}

void UTextCollectionDataItem::SetMinValue(int newValue)
{
	UMenuCollectionDataInt::SetMinValue(newValue);
}

void UTextCollectionDataItem::SetMaxValue(int newValue)
{
	UMenuCollectionDataInt::SetMaxValue(newValue);
}
