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
	// Generic
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilities(const FGameplayTagContainer &_TagContainer);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbility(const FGameplayTag &_Tag);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbilities(const FGameplayTagContainer &_TagContainer);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbility(const FGameplayTag &_Tag);

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


/////////// Respawn
public:
	virtual FVector GetRespawnPoint() override;
};