// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHealthDelegateInterface.h"

// Add default functionality here for any IPlayerHealthDelegateInterface functions that are not pure virtual.

void IPlayerHealthDelegateInterface::OnPlayerHealthChanged(float newValue)
{
	for (auto PlayerHealthInterface : PlayerHealthDelegates)
	{
		PlayerHealthInterface->OnPlayerHealthChanged(newValue);
	}
}

void IPlayerHealthDelegateInterface::OnPlayerMaxHealthChanged(float newValue)
{
	for (auto PlayerHealthInterface : PlayerHealthDelegates)
	{
		PlayerHealthInterface->OnPlayerMaxHealthChanged(newValue);
	}
}

void IPlayerHealthDelegateInterface::AddPlayerHealthDelegate(IPlayerHealthInterface* newDelegate)
{
	PlayerHealthDelegates.AddUnique(newDelegate);
}

void IPlayerHealthDelegateInterface::RemovePlayerHealthDelegate(IPlayerHealthInterface* removeDelegate)
{
	PlayerHealthDelegates.Remove(removeDelegate);
}
