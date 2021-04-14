#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerPrestigeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerPrestigeInterface : public UInterface
{
	GENERATED_BODY()
};

class SQUADLEADER_API IPlayerPrestigeInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnPlayerPrestigeChanged(float newValue) = 0;

	UFUNCTION()
	virtual void OnPlayerPrestigeLevelUpChanged(float newValue) = 0;
};
