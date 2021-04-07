#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MinimapInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMinimapInterface : public UInterface
{
	GENERATED_BODY()
};

class SQUADLEADER_API IMinimapInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnTeamPositionsChanged(const TArray<FVector2D>& _Positions) = 0;
};
