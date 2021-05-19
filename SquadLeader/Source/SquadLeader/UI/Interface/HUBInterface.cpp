// Fill out your copyright notice in the Description page of Project Settings.


#include "HUBInterface.h"

// Add default functionality here for any IHUBInterface functions that are not pure virtual.

void IHUBInterface::ClearPlayerList1()
{
	BroadCastMethod<IHUBInterface>(&IHUBInterface::ClearPlayerList1, HUBDelegates);
}

void IHUBInterface::ClearPlayerList2()
{
	BroadCastMethod<IHUBInterface>(&IHUBInterface::ClearPlayerList2, HUBDelegates);
}

void IHUBInterface::AddPlayerList1(TMap<FString, FString> statsIn)
{
	BroadCastMethod<IHUBInterface, TMap<FString, FString>>(&IHUBInterface::AddPlayerList1, HUBDelegates, statsIn);
}

void IHUBInterface::AddPlayerList2(TMap<FString, FString> statsIn)
{
	BroadCastMethod<IHUBInterface, TMap<FString, FString>>(&IHUBInterface::AddPlayerList2, HUBDelegates, statsIn);
}

void IHUBInterface::AddHUBDelegate(IHUBInterface* newDelegate)
{
	HUBDelegates.AddUnique(newDelegate);
}

void IHUBInterface::RemoveHUBsDelegate(IHUBInterface* removeDelegate)
{
	HUBDelegates.Remove(removeDelegate);
}
