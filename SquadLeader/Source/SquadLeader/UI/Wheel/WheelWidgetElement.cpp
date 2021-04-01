// Fill out your copyright notice in the Description page of Project Settings.


#include "WheelWidgetElement.h"

void UWheelWidgetElement::SetItemHovered(bool bIsHovered)
{
	if (bIsHovered != bIsItemHovered)
	{
		bIsItemHovered = bIsHovered;
	}
}

bool UWheelWidgetElement::IsItemHovered()
{
	return bIsItemHovered;
}
