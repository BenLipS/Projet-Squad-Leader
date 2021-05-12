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
				if (auto DataInt = Cast<UMenuCollectionDataInt>(PanelChild); DataInt)
				{
					DataInts.Add(DataInt->GetName(), DataInt);
				}
				else if (auto DataString = Cast<UMenuCollectionDataText>(PanelChild); DataString)
				{
					DataStrings.Add(DataString->GetName(), DataString);
				}
				else if (auto DataBool = Cast<UMenuCollectionDataBool>(PanelChild); DataBool)
				{
					DataBools.Add(DataBool->GetName(), DataBool);
				}
			}
		}
	}

	InitIntValues();
}

void UMenuDataCollectionWidget::OnItemAddedToLayout_Implementation()
{
	
}

void UMenuDataCollectionWidget::InitIntValues()
{
	if (IsValid(MinValues))
	{
		auto listInts = MinValues->GetDefaultObject<AGameParam>()->GetIntParams();

		for (auto IntPairValue : listInts)
		{
			if (auto IntDataContainer = DataInts.Find(IntPairValue.Key); IntDataContainer)
			{
				(*IntDataContainer)->SetMinValue(IntPairValue.Value);
			}
		}
	}

	if (IsValid(MaxValues))
	{
		auto listInts = MaxValues->GetDefaultObject<AGameParam>()->GetIntParams();

		for (auto IntPairValue : listInts)
		{
			if (auto IntDataContainer = DataInts.Find(IntPairValue.Key); IntDataContainer)
			{
				(*IntDataContainer)->SetMaxValue(IntPairValue.Value);
			}
		}
	}

	ResyncDefaultIntvalues();
}

void UMenuDataCollectionWidget::ResyncDefaultIntvalues() {
	if (IsValid(DefaultValues))
	{
		auto listInts = DefaultValues->GetDefaultObject<AGameParam>()->GetIntParams();

		for (auto IntPairValue : listInts)
		{
			if (auto IntDataContainer = DataInts.Find(IntPairValue.Key); IntDataContainer)
			{
				(*IntDataContainer)->SetDataValue(IntPairValue.Value);
			}
		}
	}
}

void UMenuDataCollectionWidget::RandomizeIntValues()
{
}


TMap<FString, int> UMenuDataCollectionWidget::GetIntData()
{
	TMap<FString, int> IntData;

	for (auto DataItem : DataInts)
	{
		int value = DataItem.Value->GetDataValue();
		IntData.Add(DataItem.Key, value);
	}
	return IntData;
}

TMap<FString, FString> UMenuDataCollectionWidget::GetStringData()
{
	TMap<FString, FString> StringData;

	for (auto DataItem : DataStrings)
	{
		if (FString value = DataItem.Value->GetDataValue(); !value.IsEmpty()) {
			StringData.Add(DataItem.Key, value);
		}
	}
	return StringData;
}
