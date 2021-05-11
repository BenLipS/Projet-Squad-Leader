// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCollectionDataCheckBox.h"

void UMenuCollectionDataCheckBox::SynchronizeProperties()
{
	if (IsValid(CheckBoxData))
	{
		CheckBoxData->SetCheckedState(DataValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

		CheckBoxData->OnCheckStateChanged.AddDynamic(this, &UMenuCollectionDataCheckBox::OnCheckBoxChanged);
	}
}

void UMenuCollectionDataCheckBox::OnCheckBoxChanged(bool newValue)
{
	SetDataValue(newValue);
}

bool UMenuCollectionDataCheckBox::SetDataValue(bool newValue)
{
	if (UMenuCollectionDataBool::SetDataValue(newValue))
	{
		if (IsValid(CheckBoxData))
		{
			CheckBoxData->SetCheckedState(DataValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
		}
		return true;
	}
	return false;
}
