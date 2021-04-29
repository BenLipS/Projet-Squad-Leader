#include "MinimapDelegateInterface.h"

// Add default functionality here for any IMinimapDelegateInterface functions that are not pure virtual.

void IMinimapDelegateInterface::OnSoldierAddedToTeam(ASoldier* _Soldier)
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnSoldierAddedToTeam(_Soldier);
	}
}

void IMinimapDelegateInterface::OnPingAdded(FVector2D PosPingActor)
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnPingAdded(PosPingActor);
	}
}

void IMinimapDelegateInterface::OnSoldierRemovedFromTeam(ASoldier* _Soldier)
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnSoldierRemovedFromTeam(_Soldier);
	}
}

void IMinimapDelegateInterface::OnControlAreaAdded(AControlArea* _ControlArea)
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnControlAreaAdded(_ControlArea);
	}
}

void IMinimapDelegateInterface::OnUpdatePOIs()
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnUpdatePOIs();
	}
}

void IMinimapDelegateInterface::OnFullMapDisplayBegin()
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnFullMapDisplayBegin();
	}
}

void IMinimapDelegateInterface::OnFullMapDisplayEnd()
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnFullMapDisplayEnd();
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
