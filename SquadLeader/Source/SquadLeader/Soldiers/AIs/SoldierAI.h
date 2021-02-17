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
};