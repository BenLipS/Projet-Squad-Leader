// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaWidget.h"

#include "Interface/MinimapDelegateInterface.h"
#include "Interface/TicketDelegateInterface.h"
#include "SquadLeader/GameState/SquadLeaderGameState.h"
#include "../Soldiers/SoldierTeam.h"
#include "../Soldiers/Players/SoldierPlayerController.h"

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
	}
}

void UControlAreaWidget::OnEnnemyTicketChanged(int newTicket)
{
	if (RightTicketText)
	{
		RightTicketText->SetText(FText::FromString(FString::FromInt(newTicket)));
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
			ControlAreaList.Add(newWidget);
			AreaContainer->AddChildToHorizontalBox(newWidget);
		}
	}
}

void UControlAreaWidget::OnControlAreaAdded(AControlArea* ControlArea)
{
	if (ControlAreaInfoClass)
	{
		UControlAreaInfoWidget* newWidget = WidgetTree->ConstructWidget<UControlAreaInfoWidget>(ControlAreaInfoClass);
		newWidget->OnControlAreaOwnerChange(0);
		newWidget->OnControlAreaCapturerChange(0);
		newWidget->OnControlAreaPercentageChange(0.f);
		ControlAreaList.Add(newWidget);
		AreaContainer->AddChildToHorizontalBox(newWidget);
		ControlArea->OnOwnerChanged.AddDynamic(newWidget, &UControlAreaInfoWidget::OnControlAreaOwnerChange);
		ControlArea->OnCapturerChanged.AddDynamic(newWidget, &UControlAreaInfoWidget::OnControlAreaCapturerChange);
		ControlArea->OnPercentageChanged.AddDynamic(newWidget, &UControlAreaInfoWidget::OnControlAreaPercentageChange);
	}
}
//
//auto playerController = GetOwningPlayer<ASoldierPlayerController>();
//
//if (playerController)
//{
//	auto teams = playerController->GetWorld()->GetGameState<ASquadLeaderGameState>()->GetSoldierTeamCollection();
//
//	for (ASoldierTeam* team : teams)
//	{
//		if (team->Id == playerController->GetTeam()->Id)
//		{
//			team->OnTicketChanged.AddDynamic(this, &UControlAreaWidget::OnAllyTicketChanged);
//			team->BroadcastTickets();
//		}
//		else if (team->Id < 3)
//		{
//			team->OnTicketChanged.AddDynamic(this, &UControlAreaWidget::OnEnnemyTicketChanged);
//			team->BroadcastTickets();
//		}
//	}
//}