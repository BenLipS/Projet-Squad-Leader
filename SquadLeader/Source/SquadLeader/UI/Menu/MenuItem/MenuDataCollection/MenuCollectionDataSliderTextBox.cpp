// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCollectionDataSliderTextBox.h"

void UMenuCollectionDataSliderTextBox::SynchronizeProperties()
{
    UMenuCollectionDataInt::SynchronizeProperties();

    if (IsValid(TextBoxInt))
    {
        TextBoxInt->SetDataValue(DataValue - 1, false);
        TextBoxInt->SetMinValue(MinValue - 1);
        TextBoxInt->SetMaxValue(MaxValue - 1);

        TextBoxInt->SetDataValue(DataValue, true);
        TextBoxInt->SetMinValue(MinValue);
        TextBoxInt->SetMaxValue(MaxValue);
    }

    if (IsValid(SliderInt))
    {
        SliderInt->SetDataValue(DataValue - 1, true);
        SliderInt->SetMinValue(MinValue - 1);
        SliderInt->SetMaxValue(MaxValue - 1);

        SliderInt->SetDataValue(DataValue, true);
        SliderInt->SetMinValue(MinValue);
        SliderInt->SetMaxValue(MaxValue);
    }

    if (IsValid(TextName))
    {
        TextName->SetText(FText::FromString(TextWidget + "yo"));
        TextName->SetText(FText::FromString(TextWidget));
    }
}

bool UMenuCollectionDataSliderTextBox::SetDataValue(int newValue, bool fireEvent)
{
    if(UMenuCollectionDataInt::SetDataValue(newValue, fireEvent))
    {
        if (IsValid(TextBoxInt))
        {
            TextBoxInt->SetDataValue(newValue, fireEvent);
        }
        if (IsValid(SliderInt))
        {
            SliderInt->SetDataValue(newValue, fireEvent);
        }
        return true;
    }
    return false;
}

bool UMenuCollectionDataSliderTextBox::SetMinValue(int newValue)
{
    if (UMenuCollectionDataInt::SetMinValue(newValue))
    {
        if (IsValid(TextBoxInt))
        {
            TextBoxInt->SetMinValue(newValue);
        }
        if (IsValid(SliderInt))
        {
            SliderInt->SetMinValue(newValue);
        }
        return true;
    }
    return false;
}

bool UMenuCollectionDataSliderTextBox::SetMaxValue(int newValue)
{
    if (UMenuCollectionDataInt::SetMaxValue(newValue))
    {
        if (IsValid(TextBoxInt))
        {
            TextBoxInt->SetMaxValue(newValue);
        }
        if (IsValid(SliderInt))
        {
            SliderInt->SetMaxValue(newValue);
        }
        return true;
    }
    return false;
}

void UMenuCollectionDataSliderTextBox::SetText(const FString& TextWidgetIn)
{
    if (IsValid(TextName))
    {
        TextName->SetText(FText::FromString(TextWidgetIn));
    }
}

FString UMenuCollectionDataSliderTextBox::GetText()
{
    if (IsValid(TextName))
    {
        return TextName->GetText().ToString();
    }
    return "";
}
