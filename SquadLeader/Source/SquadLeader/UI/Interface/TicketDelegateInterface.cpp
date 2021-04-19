// Fill out your copyright notice in the Description page of Project Settings.


#include "TicketDelegateInterface.h"

// Add default functionality here for any ITicketDelegateInterface functions that are not pure virtual.

void ITicketDelegateInterface::OnAllyTicketChanged(int newTicket)
{
	for (auto TicketInterface : TicketDelegates)
	{
		TicketInterface->OnAllyTicketChanged(newTicket);
	}
}

void ITicketDelegateInterface::OnEnnemyTicketChanged(int newTicket)
{
	for (auto TicketInterface : TicketDelegates)
	{
		TicketInterface->OnEnnemyTicketChanged(newTicket);
	}
}

void ITicketDelegateInterface::AddTicketDelegate(ITicketInterface* newDelegate)
{
	TicketDelegates.AddUnique(newDelegate);
}

void ITicketDelegateInterface::RemoveTicketDelegate(ITicketInterface* removeDelegate)
{
	TicketDelegates.Remove(removeDelegate);
}
