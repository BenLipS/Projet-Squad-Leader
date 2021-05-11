// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCollectionDataInt.h"

void UMenuCollectionDataInt::SetDataValue(int newValue)
{
    newValue = FMath::Clamp<int>(newValue, MinValue, MaxValue);

    if (newValue != DataValue)
    {
        DataValue = newValue;
        OnValueChangedEvent.Broadcast(DataValue);
    }
}

int UMenuCollectionDataInt::GetDataValue()
{
    return DataValue;
}

void UMenuCollectionDataInt::SetMinValue(int newValue)
{
    MinValue = newValue;
    if (newValue > MaxValue)
    {
        SetMaxValue(MinValue);
    }

    if (newValue > DataValue)
    {
        SetDataValue(MinValue);
    }
}

int UMenuCollectionDataInt::GetMinValue()
{
    return MinValue;
}

void UMenuCollectionDataInt::SetMaxValue(int newValue)
{
    MaxValue = newValue;
    if (newValue < MaxValue)
    {
        SetMinValue(MaxValue);
    }

    if (newValue < DataValue)
    {
        SetDataValue(MaxValue);
    }
}

int UMenuCollectionDataInt::GetMaxValue()
{
    return MaxValue;
}
