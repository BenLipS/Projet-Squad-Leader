// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShieldDelegateInterface.h"

// Add default functionality here for any IPlayerShieldDelegateInterface functions that are not pure virtual.

void IPlayerShieldDelegateInterface::OnPlayerShieldChanged(float newValue)
{
	for (auto PlayerShieldInterface : PlayerShieldDelegates)
	{
		PlayerShieldInterface->OnPlayerShieldChanged(newValue);
	}
}

void IPlayerShieldDelegateInterface::OnPlayerMaxShieldChanged(float newValue)
{
	for (auto PlayerShieldInterface : PlayerShieldDelegates)
	{
		PlayerShieldInterface->OnPlayerMaxShieldChanged(newValue);
	}
}

void IPlayerShieldDelegateInterface::AddPlayerShieldDelegate(IPlayerShieldInterface* newDelegate)
{
	PlayerShieldDelegates.AddUnique(newDelegate);
}

void IPlayerShieldDelegateInterface::RemovePlayerShieldDelegate(IPlayerShieldInterface* removeDelegate)
{
	PlayerShieldDelegates.Remove(removeDelegate);
}
