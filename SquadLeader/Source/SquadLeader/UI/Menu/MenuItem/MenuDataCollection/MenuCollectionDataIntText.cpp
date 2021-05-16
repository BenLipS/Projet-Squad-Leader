// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCollectionDataIntText.h"

void UMenuCollectionDataIntText::SynchronizeProperties()
{
	UMenuCollectionDataInt::SynchronizeProperties();

	if (IsValid(TextInt))
	{
		TextInt->SetText(FText::FromString(FString::FromInt(DataValue)));
	}

	if (IsValid(DescriptionText))
	{
		DescriptionText->SetText(FText::FromString(Description));
	}
}

bool UMenuCollectionDataIntText::SetDataValue(int newValue, bool fireEvent)
{
	if (SetDataValue(newValue, fireEvent))
	{
		TextInt->SetText(FText::FromString(FString::FromInt(DataValue)));
		return true;
	}
	return false;
}
