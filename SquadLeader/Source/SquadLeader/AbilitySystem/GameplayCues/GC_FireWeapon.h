#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GC_FireWeapon.generated.h"

UCLASS()
class SQUADLEADER_API UGC_FireWeapon : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

protected:
	UGC_FireWeapon(const FObjectInitializer& _ObjectInitializer);
};
