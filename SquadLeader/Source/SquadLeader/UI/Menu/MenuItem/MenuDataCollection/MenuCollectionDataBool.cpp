// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCollectionDataBool.h"

bool UMenuCollectionDataBool::SetDataValue(bool newValue)
{
    if (DataValue != newValue)
    {
        DataValue = newValue;
        OnValueChangedEvent.Broadcast(DataValue);

        return true;
    }
    return false;
}

bool UMenuCollectionDataBool::GetDataValue()
{
    return DataValue;;
}
