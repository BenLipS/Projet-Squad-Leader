// Fill out your copyright notice in the Description page of Project Settings.


#include "WheelWidgetElement.h"

void UWheelWidgetElement::SetItemHovered(bool bIsHovered)
{
	if (bIsHovered != bIsItemHovered)
	{
		bIsItemHovered = bIsHovered;
		if (bIsItemHovered)
		{
			SetRenderScale(FVector2D(1.25f));
		}
		else
		{
			SetRenderScale(FVector2D(1.f));
		}
	}
}

bool UWheelWidgetElement::IsItemHovered()
{
	return bIsItemHovered;
}
