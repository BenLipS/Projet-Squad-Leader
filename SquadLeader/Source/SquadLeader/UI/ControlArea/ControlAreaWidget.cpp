// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaWidget.h"

#include "../Interface/MinimapDelegateInterface.h"
#include "../Interface/TicketDelegateInterface.h"

#include "SquadLeader/GameState/SquadLeaderGameState.h"
#include "SquadLeader/Soldiers/SoldierTeam.h"
#include "SquadLeader/Soldiers/Players/SoldierPlayerController.h"

#include "ControlAreaInfoWidget.h"

#include "Blueprint/WidgetTree.h"

void UControlAreaWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (IMinimapDelegateInterface* MinimapDelegateInterface = Cast<IMinimapDelegateInterface>(ObjectIn); MinimapDelegateInterface)
	{
		MinimapDelegateInterface->AddMinimapDelegate(this);
	}

	if (ITicketDelegateInterface* TicketDelegateInterface = Cast<ITicketDelegateInterface>(ObjectIn); TicketDelegateInterface)
	{
		TicketDelegateInterface->AddTicketDelegate(this);
	}
}

void UControlAreaWidget::OnAllyTicketChanged(int newTicket)
{
	if (LeftTicketText)
	{
		LeftTicketText->SetText(FText::FromString(FString::FromInt(newTicket)));

		if (IsValid(LeftGlow))
		{
			PlayAnimation(LeftGlow);
		}
	}
}

void UControlAreaWidget::OnEnnemyTicketChanged(int newTicket)
{
	if (RightTicketText)
	{
		RightTicketText->SetText(FText::FromString(FString::FromInt(newTicket)));

		if (IsValid(RightGlow))
		{
			PlayAnimation(RightGlow);
		}
	}
}

void UControlAreaWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (IsDesignTime())
	{
		if (ControlAreaInfoClass)
		{
			UControlAreaInfoWidget* newWidget = WidgetTree->ConstructWidget<UControlAreaInfoWidget>(ControlAreaInfoClass);
			newWidget->OnControlAreaOwnerChange(1);
			newWidget->OnControlAreaCapturerChange(1);
			newWidget->OnControlAreaPercentageChange(1.f);
			AreaContainer->AddChildToHorizontalBox(newWidget);
		}
	}
}

void UControlAreaWidget::OnControlAreaAdded(AControlArea* newControlArea)
{
	for (auto ControlArea : ControlAreaList)
	{
		if (ControlArea == newControlArea)
		{
			return;
		}
	}
	if (ControlAreaInfoClass)
	{
		UControlAreaInfoWidget* newWidget = WidgetTree->ConstructWidget<UControlAreaInfoWidget>(ControlAreaInfoClass);
		newWidget->OnControlAreaOwnerChange(0);
		newWidget->OnControlAreaCapturerChange(0);
		newWidget->OnControlAreaPercentageChange(0.f);
		newWidget->OnControlAreaNameChange(newControlArea->ControlAreaName);
		ControlAreaList.Add(newControlArea);
		AreaContainer->AddChildToHorizontalBox(newWidget);
		newControlArea->OnOwnerChanged.AddDynamic(newWidget, &UControlAreaInfoWidget::OnControlAreaOwnerChange);
		newControlArea->OnCapturerChanged.AddDynamic(newWidget, &UControlAreaInfoWidget::OnControlAreaCapturerChange);
		newControlArea->OnPercentageChanged.AddDynamic(newWidget, &UControlAreaInfoWidget::OnControlAreaPercentageChange);
	}
}