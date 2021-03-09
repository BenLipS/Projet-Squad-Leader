#pragma once

#include "CoreMinimal.h"
#include "../Soldier.h"
#include "SoldierAI.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIHealthChanged, float, newValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIMaxHealthChanged, float, newValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIShieldChanged, float, newValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIMaxShieldChanged, float, newValue);

UCLASS()
class SQUADLEADER_API ASoldierAI : public ASoldier
{
	GENERATED_BODY()
	
public:
	ASoldierAI(const FObjectInitializer& _ObjectInitializer);
	void BroadCastDatas();

public:
	//-----DELEGATE-----
	FAIHealthChanged OnHealthChanged;
	FAIMaxHealthChanged OnMaxHealthChanged;
	FAIShieldChanged OnShieldChanged;
	FAIMaxShieldChanged OnMaxShieldChanged;
protected:

	virtual void BeginPlay() override;

	virtual FVector lookingAtPosition() override;

	FVector LookingAtPosition;

//////////////// Teamable
public:
	virtual TSubclassOf<ASoldierTeam> GetTeam() override;
	virtual bool SetTeam(TSubclassOf<ASoldierTeam> _Team) override;

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
	void SetLookingAtPosition(const FVector& _LookingAtPosition);

	virtual void Die() override;

	void InitializeAttributeChangeCallbacks() override;

	//-----Delegate-----	
protected:

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ShieldChangedDelegateHandle;
	FDelegateHandle MaxShieldChangedDelegateHandle;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ShieldChanged(const FOnAttributeChangeData& Data);
	virtual void MaxShieldChanged(const FOnAttributeChangeData& Data);

/////////// Respawn
public:
	virtual void Respawn() override;
	virtual FVector GetRespawnPoint() override;
};