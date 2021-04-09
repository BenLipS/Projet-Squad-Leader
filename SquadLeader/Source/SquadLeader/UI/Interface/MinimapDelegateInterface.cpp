#include "MinimapDelegateInterface.h"

// Add default functionality here for any IMinimapDelegateInterface functions that are not pure virtual.

void IMinimapDelegateInterface::OnSoldierAddedToTeam(ASoldier* _Soldier)
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnSoldierAddedToTeam(_Soldier);
	}
}

void IMinimapDelegateInterface::OnSoldierRemovedFromTeam(ASoldier* _Soldier)
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnSoldierRemovedFromTeam(_Soldier);
	}
}

void IMinimapDelegateInterface::OnUpdateTeamPositions()
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnUpdateTeamPositions();
	}
}

void IMinimapDelegateInterface::AddMinimapDelegate(IMinimapInterface* _NewDelegate)
{
	MinimapDelegates.AddUnique(_NewDelegate);
}

void IMinimapDelegateInterface::RemoveMinimapDelegate(IMinimapInterface* _RemoveDelegate)
{
	MinimapDelegates.Remove(_RemoveDelegate);
}
