#include "MinimapDelegateInterface.h"

// Add default functionality here for any IMinimapDelegateInterface functions that are not pure virtual.

void IMinimapDelegateInterface::OnTeamPositionsChanged(const TArray<FVector2D>& _Positions)
{
	for (auto MinimapInterface : MinimapDelegates)
	{
		MinimapInterface->OnTeamPositionsChanged(_Positions);
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
