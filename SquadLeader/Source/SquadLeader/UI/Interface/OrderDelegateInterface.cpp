// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderDelegateInterface.h"

// Add default functionality here for any IOrderDelegateInterface functions that are not pure virtual.

void IOrderDelegateInterface::OnOrderInputPressed()
{
	for (auto OrderInterface : OrderDelegates)
	{
		OrderInterface->OnOrderInputPressed();
	}
}

void IOrderDelegateInterface::OnOrderInputReleased()
{
	for (auto OrderInterface : OrderDelegates)
	{
		OrderInterface->OnOrderInputPressed();
	}
}

void IOrderDelegateInterface::AddPlayerHealthDelegate(IOrderInterface* newDelegate)
{
	OrderDelegates.AddUnique(newDelegate);
}

void IOrderDelegateInterface::RemovePlayerHealthDelegate(IOrderInterface* removeDelegate)
{
	OrderDelegates.Remove(removeDelegate);
}