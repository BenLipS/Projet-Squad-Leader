// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCollectionDataTextBoxText.h"

void UMenuCollectionDataTextBoxText::SynchronizeProperties()
{
	UMenuCollectionDataText::SynchronizeProperties();

}

bool UMenuCollectionDataTextBoxText::SetDataValue(const FString& newValue, bool fireEvent)
{
	if (UMenuCollectionDataText::SetDataValue(newValue, fireEvent))
	{
		TextBoxData->SetText(FText::FromString(DataValue));
		return true;
	}
	return false;
}
