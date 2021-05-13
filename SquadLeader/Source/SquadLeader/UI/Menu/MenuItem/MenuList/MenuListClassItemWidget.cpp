// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuListClassItemWidget.h"

#include "MenuListClass.h"

void UMenuListClassItemWidget::SynchronizeProperties()
{
	UMenuListInfoItemWidget::SynchronizeProperties();

	ClassButton->OnClicked.AddDynamic(this, &UMenuListClassItemWidget::OnItemSelect);
}

void UMenuListClassItemWidget::SetPlayerClass(SoldierClass newClass)
{
	CurrentClass = newClass;
	if (auto ClassList = Cast<UMenuListClass>(OwnerList); IsValid(ClassList))
	{
		auto newClassAsset = ClassList->GetAssetFromClass(CurrentClass);
		SetRightData(ASoldier::SoldierClassToStr(CurrentClass));
		if (IsValid(newClassAsset.Icon) && IsValid(IconImage))
		{
			IconImage->SetBrushFromAsset(newClassAsset.Icon);
		}

		if (IsValid(newClassAsset.Background) && IsValid(Background))
		{
			Background->SetBrushFromAsset(newClassAsset.Background);
		}
	}
}

void UMenuListClassItemWidget::OnItemSelect()
{
	Super::OnItemSelect();
	if (auto classList = Cast<UMenuListClass>(OwnerList); IsValid(classList))
	{
		classList->CurrentClass = CurrentClass;

		classList->SetAssetFromClass(classList->CurrentClass);

		classList->OnClassButtonPressed();
	}
}

void UMenuListClassItemWidget::OnItemDeselect()
{
}
