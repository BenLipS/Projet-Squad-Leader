#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MinimapInterface.h"
#include "MinimapDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMinimapDelegateInterface : public UMinimapInterface
{
	GENERATED_BODY()
};

class SQUADLEADER_API IMinimapDelegateInterface : public IMinimapInterface
{
	GENERATED_BODY()

private:
	TArray<IMinimapInterface*> MinimapDelegates;

public:
	virtual void OnTeamPositionsChanged(const TArray<FVector2D>& _Positions);

	void AddMinimapDelegate(IMinimapInterface* _NewDelegate);
	void RemoveMinimapDelegate(IMinimapInterface* _RemoveDelegate);
};