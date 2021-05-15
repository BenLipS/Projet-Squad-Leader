// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCollectionDataText.h"

bool UMenuCollectionDataText::SetDataValue(const FString& newValue, bool fireEvent)
{
	if (newValue != DataValue)
	{
		DataValue = newValue;
		if (fireEvent)
		{
			OnValueChangedEvent.Broadcast(DataValue);
		}
		return true;
	}
	return false;
}

FString UMenuCollectionDataText::GetDataValue()
{
	return DataValue;
}