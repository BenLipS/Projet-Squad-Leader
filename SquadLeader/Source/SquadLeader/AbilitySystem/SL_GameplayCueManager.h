#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "SL_GameplayCueManager.generated.h"

UCLASS()
class SQUADLEADER_API USL_GameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()
	
protected:
	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;
};
