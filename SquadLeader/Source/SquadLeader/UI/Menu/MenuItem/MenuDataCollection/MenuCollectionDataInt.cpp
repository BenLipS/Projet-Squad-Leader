// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCollectionDataInt.h"

bool UMenuCollectionDataInt::SetDataValue(int newValue, bool fireEvent)
{
    newValue = FMath::Clamp<int>(newValue, MinValue, MaxValue);

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

int UMenuCollectionDataInt::GetDataValue()
{
    return DataValue;
}

bool UMenuCollectionDataInt::SetMinValue(int newValue)
{
    if (newValue != MinValue)
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

        return true;
    }
    return false;
}

int UMenuCollectionDataInt::GetMinValue()
{
    return MinValue;
}

bool UMenuCollectionDataInt::SetMaxValue(int newValue)
{
    if (newValue != MaxValue)
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
        return true;
    }
    return false;
}

int UMenuCollectionDataInt::GetMaxValue()
{
    return MaxValue;
}
