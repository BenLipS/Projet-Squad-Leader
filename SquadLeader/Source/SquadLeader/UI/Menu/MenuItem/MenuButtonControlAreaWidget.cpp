// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuButtonControlAreaWidget.h"

#include "SquadLeader/ControlArea/ControlArea.h"
#include "SquadLeader/UI/ControlArea/ControlAreaInfoWidget.h"
#include "../MenuLayoutWidget.h"

#include "Components/Image.h"

void UMenuButtonControlAreaWidget::SetControlAreaOwner(int newOwner)
{
	if (ControlAreaOwner != newOwner)
	{
		ControlAreaOwner = newOwner;
		if (ControlAreaOwner > 0)
		{
			bIsSelectGlobal = true;
		}
		else
		{
			bIsSelectGlobal = false;
			MenuLayout->DeselectItem(this);
		}
	}
}

void UMenuButtonControlAreaWidget::OnItemSelected()
{
	UMenuButtonWidget::OnItemSelected();
	Background->SetVisibility(ESlateVisibility::Visible);
}

void UMenuButtonControlAreaWidget::OnItemDeselected()
{
	UMenuButtonWidget::OnItemDeselected();
	Background->SetVisibility(ESlateVisibility::Hidden);
}

int UMenuButtonControlAreaWidget::GetControlAreaOwner()
{
	return ControlAreaOwner;
}

void UMenuButtonControlAreaWidget::SetControlArealink(AControlArea* ControlAreaIn)
{
	if (IsValid(ControlAreaWidget))
	{
		ControlAreaIn->OnOwnerChanged.AddDynamic(ControlAreaWidget, &UControlAreaInfoWidget::OnControlAreaOwnerChange);
		ControlAreaIn->OnCapturerChanged.AddDynamic(ControlAreaWidget, &UControlAreaInfoWidget::OnControlAreaCapturerChange);
		ControlAreaIn->OnPercentageChanged.AddDynamic(ControlAreaWidget, &UControlAreaInfoWidget::OnControlAreaPercentageChange);
	}
	if (ControlAreaOwner <= 0)
	{
		bIsSelectGlobal = false;
	}
	ControlAreaIn->OnOwnerChanged.AddDynamic(this, &UMenuButtonControlAreaWidget::SetControlAreaOwner);
	ControlAreaIn->BroadcastDatas();
	ControlArea = ControlAreaIn;
}

AControlArea* UMenuButtonControlAreaWidget::GetControlArea()
{
	return ControlArea;
}

void UMenuButtonControlAreaWidget::SynchronizeProperties()
{
	UMenuButtonWidget::SynchronizeProperties();
	if (IsValid(MainButton))
	{
		//MainButton->OnClicked.AddDynamic(this, &UMenuButtonControlAreaWidget::OnButtonPressed);
	}
}