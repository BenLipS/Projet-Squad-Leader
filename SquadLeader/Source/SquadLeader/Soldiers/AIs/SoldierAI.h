#pragma once

#include "CoreMinimal.h"
#include "../Soldier.h"
#include "SoldierAI.generated.h"

UCLASS()
class SQUADLEADER_API ASoldierAI : public ASoldier
{
	GENERATED_BODY()
	
public:
	ASoldierAI(const FObjectInitializer& _ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	virtual FVector lookingAtPosition() override;

	FVector LookingAtPosition;

//////////////// Abilities
public:
	// Fire
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilityFire();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbilityFire();

	// Run
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilityRun();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbilityRun();

	UFUNCTION(BlueprintCallable, Category = "Shoot")
	void SetLookingAtPosition(FVector lookingAtPosition);

	virtual void Die() override;

/////////// Respawn
public:
	virtual FVector GetRespawnPoint() override;
};