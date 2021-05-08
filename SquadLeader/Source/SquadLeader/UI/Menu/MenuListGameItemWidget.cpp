// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuListGameItemWidget.h"

void UMenuListGameItemWidget::SetIPAdress(FString AdressIn)
{
	IPAdress = AdressIn;
}

FString UMenuListGameItemWidget::GetIPAdress()
{
	return IPAdress;
}

void UMenuListGameItemWidget::OnItemSelect()
{
	Super::OnItemSelect();
	/*if (IsValid(LeftData))
	{
		LeftData->SetColorAndOpacity(FSlateColor(FLinearColor(0.f, 0.f, 0.f)));
	}

	if (IsValid(LeftData))
	{
		RightData->SetColorAndOpacity(FSlateColor(FLinearColor(0.f, 0.f, 0.f)));
	}*/

	Background->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.5f));
}

void UMenuListGameItemWidget::OnItemDeselect()
{
	Super::OnItemDeselect();
	/*if (IsValid(LeftData))
	{
		LeftData->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
	}

	if (IsValid(LeftData))
	{
		RightData->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
	}*/

	Background->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.2f));
}
