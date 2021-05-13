// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuListClass.h"
#include "MenuListClassItemWidget.h"

#include "Blueprint/WidgetTree.h"

void UMenuListClass::SynchronizeProperties()
{
	UMenuListInfo::SynchronizeProperties();

	ListEntry->ClearChildren();

//-----Self-----
	SetAssetFromClass(CurrentClass);

	ClassButton->OnClicked.AddDynamic(this, &UMenuListClass::OnClassButtonPressed);

//-----Items-----
	//ItemClass exist ?
	if (ItemClass)
	{
		//Is ItemClass a subclass of UMenuListClassItemWidget ?
		//For all playable class
		for (auto ClassItem : ASoldier::GetAllPlayableClass())
		{
			//Construct widget
			auto newItem = WidgetTree->ConstructWidget<UMenuListClassItemWidget>(ItemClass);

			if (IsValid(newItem))
			{
				AddEntryToList(newItem);
				//Setup class and assets
				newItem->SetPlayerClass(ClassItem);
			}
		}
	}

	if (!IsDesignTime())
	{
		ListEntry->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FSoldierIconAsset UMenuListClass::GetAssetFromClass(SoldierClass SoldierClassIn)
{
	if (ClassAsset.Contains(SoldierClassIn))
	{
		return ClassAsset[SoldierClassIn];
	}
	else if (ClassAsset.Contains(SoldierClass::NONE))
	{
		return ClassAsset[SoldierClass::NONE];
	}
	else
	{
		return FSoldierIconAsset();
	}
}

void UMenuListClass::SetAssetFromClass(SoldierClass SoldierClassIn)
{
	auto IconAsset = GetAssetFromClass(SoldierClassIn);

	if (IsValid(IconAsset.Icon) && IsValid(IconImage))
	{
		IconImage->SetBrushFromAsset(IconAsset.Icon);
	}

	if (IsValid(IconAsset.Background) && IsValid(IconBackground))
	{
		IconBackground->SetBrushFromAsset(IconAsset.Background);
	}
}

void UMenuListClass::OnClassButtonPressed()
{
	bIsActive = !bIsActive;
	ListEntry->SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}