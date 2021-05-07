#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "../Interface/MenuGameInfoDelegateInterface.h"
#include "HUBMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UHUBMenuWidget : public UMenuWidget, public IMenuGameInfoDelegateInterface
{
	GENERATED_BODY()

public:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn);

//-----MenuGameInfoDelegateInterface-----
	//virtual void OnGameInfoReceived(FString Key, FString Value, FString IP) override;
	//virtual void OnGamesInfoCleanOrder() override;
};
