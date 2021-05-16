// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnInterface.h"

// Add default functionality here for any IRespawnInterface functions that are not pure virtual.

void IRespawnInterface::OnRespawnScreenActivated()
{
	BroadCastMethod<IRespawnInterface>(&IRespawnInterface::OnRespawnScreenActivated, RespawnDelegates);
}

void IRespawnInterface::AddRespawnDelegate(IRespawnInterface* newDelegate)
{
	RespawnDelegates.AddUnique(newDelegate);
}

void IRespawnInterface::RemoveRespawnDelegate(IRespawnInterface* removeDelegate)
{
	RespawnDelegates.Remove(removeDelegate);
}
