#pragma once

#include "CoreMinimal.h"
#include "SoldierPlayer.h"
#include "SoldierPlayerHeavy.generated.h"

UCLASS()
class SQUADLEADER_API ASoldierPlayerHeavy : public ASoldierPlayer
{
	GENERATED_BODY()
	
public:
	ASoldierPlayerHeavy(const FObjectInitializer& _ObjectInitializer);
};
