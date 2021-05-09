#pragma once

#include "CoreMinimal.h"
#include "../MenuWidget.h"
#include "HUBMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UHUBMenuWidget : public UMenuWidget
{
	GENERATED_BODY()

public:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn);
};
