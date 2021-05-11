// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuDataCollectionWidget.h"

#include "MenuCollectionDataItem.h"

void UMenuDataCollectionWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (IsValid(DataSlot))
	{
		if (auto Panel = Cast<UPanelWidget>(DataSlot->GetChildAt(0)); IsValid(Panel))
		{
			DataContainer = Panel;

			for (auto PanelChild : Panel->GetAllChildren())
			{
				if (auto DataItem = Cast<UMenuCollectionDataItem>(PanelChild); DataItem)
				{
					DataItems.Add(DataItem->GetName(), DataItem);
				}
			}
		}
	}
}

void UMenuDataCollectionWidget::OnItemAddedToLayout_Implementation()
{
	
}

/*int UMenuDataCollectionWidget::GetData(FString Key)
{
	if (auto DataItem = DataItems.Find(Key); DataItem && IsValid(*DataItem))
	{
		return (*DataItem)->GetDataValue();
	}
	else
	{
		return 0;
	}
}*/
