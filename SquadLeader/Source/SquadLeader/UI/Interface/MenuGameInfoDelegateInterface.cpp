// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameInfoDelegateInterface.h"

// Add default functionality here for any IMenuGameInfoDelegateInterface functions that are not pure virtual.

void IMenuGameInfoDelegateInterface::OnGameInfoReceived(FString Key, FString Value, FString IP)
{
	for (auto MenuGameInfoInterface : MenuGameInfoDelegates)
	{
		MenuGameInfoInterface->OnGameInfoReceived(Key, Value, IP);
	}
}

void IMenuGameInfoDelegateInterface::OnGamesInfoCleanOrder()
{
	for (auto MenuGameInfoInterface : MenuGameInfoDelegates)
	{
		MenuGameInfoInterface->OnGamesInfoCleanOrder();
	}
}

void IMenuGameInfoDelegateInterface::AddMenuGameInfoDelegate(IMenuGameInfoInterface* newDelegate)
{
	MenuGameInfoDelegates.AddUnique(newDelegate);
}

void IMenuGameInfoDelegateInterface::RemoveMenuGameInfoDelegate(IMenuGameInfoInterface* removeDelegate)
{
	MenuGameInfoDelegates.Remove(removeDelegate);
}
