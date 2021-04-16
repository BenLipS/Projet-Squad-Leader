#include "PlayerPrestigeDelegateInterface.h"

// Add default functionality here for any IPlayerPrestigeDelegateInterface functions that are not pure virtual.

void IPlayerPrestigeDelegateInterface::OnPlayerPrestigeChanged(float newValue)
{
	for (auto PlayerPrestigeInterface : PlayerPrestigeDelegates)
	{
		PlayerPrestigeInterface->OnPlayerPrestigeChanged(newValue);
	}
}

void IPlayerPrestigeDelegateInterface::OnPlayerPrestigeLevelUpChanged(float newValue)
{
	for (auto PlayerPrestigeInterface : PlayerPrestigeDelegates)
	{
		PlayerPrestigeInterface->OnPlayerPrestigeLevelUpChanged(newValue);
	}
}

void IPlayerPrestigeDelegateInterface::AddPlayerPrestigeDelegate(IPlayerPrestigeInterface* newDelegate)
{
	PlayerPrestigeDelegates.AddUnique(newDelegate);
}

void IPlayerPrestigeDelegateInterface::RemovePlayerPrestigeDelegate(IPlayerPrestigeInterface* removeDelegate)
{
	PlayerPrestigeDelegates.Remove(removeDelegate);
}
