// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuListInfoItemWidget.h"

#include "MenuListInfo.h"

void UMenuListInfoItemWidget::SetDatas(FString left, FString right)
{
	if (LeftData && RightData)
	{
		LeftData->SetText(FText::FromString(left));
		RightData->SetText(FText::FromString(right));
	}
}

void UMenuListInfoItemWidget::SetLeftData(FString left)
{
	if (LeftData)
	{
		LeftData->SetText(FText::FromString(left));
	}
}

void UMenuListInfoItemWidget::SetRightData(FString right)
{
	if (RightData)
	{
		RightData->SetText(FText::FromString(right));
	}
}

TPair<FString, FString> UMenuListInfoItemWidget::GetDatas()
{
	if (LeftData && RightData)
	{
		return TPair<FString, FString>(LeftData->GetText().ToString(), RightData->GetText().ToString());
	}
	else
	{
		return TPair<FString, FString>();
	}
}

FString UMenuListInfoItemWidget::GetLeftData()
{
	if (LeftData)
	{
		return LeftData->GetText().ToString();
	}
	else
	{
		return FString();
	}
}

FString UMenuListInfoItemWidget::GetRightData()
{
	if (RightData)
	{
		return RightData->GetText().ToString();
	}
	else
	{
		return FString();
	}
}

void UMenuListInfoItemWidget::OnItemSelect()
{
	if (IsValid(OwnerList) && OwnerList->ItemSelected != this)
	{
		if (IsValid(OwnerList->ItemSelected))
		{
			OwnerList->ItemSelected->OnItemDeselect();
		}
		OwnerList->ItemSelected = this;
	}
}

void UMenuListInfoItemWidget::OnItemDeselect()
{
	if (IsValid(OwnerList) && OwnerList->ItemSelected == this)
	{
		OwnerList->ItemSelected = nullptr;
	}
}
