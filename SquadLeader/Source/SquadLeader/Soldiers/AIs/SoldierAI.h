#pragma once

#include "CoreMinimal.h"
#include "../Soldier.h"
#include "SoldierAI.generated.h"

UCLASS()
class SQUADLEADER_API ASoldierAI : public ASoldier
{
	GENERATED_BODY()
	
public:
	ASoldierAI();

protected:
	virtual void BeginPlay() override;

//////////////// Abilities
public:
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool StartFiring();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void StopFiring();
};
