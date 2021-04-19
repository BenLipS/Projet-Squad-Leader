// Fill out your copyright notice in the Description page of Project Settings.


#include "GameEndDelegateInterface.h"

// Add default functionality here for any IGameEndDelegateInterface functions that are not pure virtual.

void IGameEndDelegateInterface::OnGameEnd(FString EndMessage)
{
	for (auto GameEndInterface : GameEndDelegates)
	{
		GameEndInterface->OnGameEnd(EndMessage);
	}
}

void IGameEndDelegateInterface::AddGameEndDelegate(IGameEndInterface* newDelegate)
{
	GameEndDelegates.AddUnique(newDelegate);
}

void IGameEndDelegateInterface::RemoveGameEndDelegate(IGameEndInterface* removeDelegate)
{
	GameEndDelegates.Remove(removeDelegate);
}
