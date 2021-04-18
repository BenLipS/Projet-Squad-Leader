// Fill out your copyright notice in the Description page of Project Settings.


#include "StatInfoDelegateInterface.h"

// Add default functionality here for any IStatInfoDelegateInterface functions that are not pure virtual.

void IStatInfoDelegateInterface::OnStatInfoReceived(FString Key, FString Value)
{
	for (auto StatInfoInterface : StatInfoDelegates)
	{
		StatInfoInterface->OnStatInfoReceived(Key, Value);
	}
}

void IStatInfoDelegateInterface::OnStatsInfoReceived(TMap<FString, FString> statsIn)
{
	for (auto StatInfoInterface : StatInfoDelegates)
	{
		StatInfoInterface->OnStatsInfoReceived(statsIn);
	}
}

void IStatInfoDelegateInterface::OnStatsInfoCleanOrder()
{
	for (auto StatInfoInterface : StatInfoDelegates)
	{
		StatInfoInterface->OnStatsInfoCleanOrder();
	}
}

void IStatInfoDelegateInterface::AddStatInfoDelegate(IStatInfoInterface* newDelegate)
{
	StatInfoDelegates.AddUnique(newDelegate);
}

void IStatInfoDelegateInterface::RemoveStatInfoDelegate(IStatInfoInterface* removeDelegate)
{
	StatInfoDelegates.Remove(removeDelegate);
}