#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerPrestigeInterface.h"
#include "PlayerPrestigeDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerPrestigeDelegateInterface : public UPlayerPrestigeInterface
{
	GENERATED_BODY()
};

class SQUADLEADER_API IPlayerPrestigeDelegateInterface : public IPlayerPrestigeInterface
{
	GENERATED_BODY()

private:
	TArray<IPlayerPrestigeInterface*> PlayerPrestigeDelegates;

public:
	virtual void OnPlayerPrestigeChanged(float newValue) override;

	virtual void OnPlayerPrestigeLevelUpChanged(float newValue) override;

	void AddPlayerPrestigeDelegate(IPlayerPrestigeInterface* newDelegate);
	void RemovePlayerPrestigeDelegate(IPlayerPrestigeInterface* removeDelegate);
};