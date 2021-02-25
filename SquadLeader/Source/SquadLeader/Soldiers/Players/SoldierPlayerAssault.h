#pragma once

#include "CoreMinimal.h"
#include "SoldierPlayer.h"
#include "SoldierPlayerAssault.generated.h"

UCLASS()
class SQUADLEADER_API ASoldierPlayerAssault : public ASoldierPlayer
{
	GENERATED_BODY()
	
public:
	ASoldierPlayerAssault(const FObjectInitializer& _ObjectInitializer);
};
