#pragma once

#include "CoreMinimal.h"
#include "../Soldier.h"
#include "SoldierAI.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAIFloatChangedController, float, newValue, AAISquadController*, controller);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIFloatChanged, float, newValue);

//For client knowledge purpose
USTRUCT()
struct FSoldierAIData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float Health;

	UPROPERTY(EditAnywhere)
	float MaxHealth;

	UPROPERTY(EditAnywhere)
	float Shield;

	UPROPERTY(EditAnywhere)
	float MaxShield;

	void OnHealthChanged(float newHealth);
	void OnMaxHealthChanged(float newMaxHealth);
	void OnShieldChanged(float newShield);
	void OnMaxShieldChanged(float newMaxShield);

	FAIFloatChanged OnHealthNotify;
	FAIFloatChanged OnMaxHealthNotify;
	FAIFloatChanged OnShieldNotify;
	FAIFloatChanged OnMaxShieldNotify;

	FSoldierAIData() = default;
};

UCLASS()
class SQUADLEADER_API ASoldierAI : public ASoldier
{
	GENERATED_BODY()
	
public:
	ASoldierAI(const FObjectInitializer& _ObjectInitializer);

//////////////// Controllers
protected:
	virtual void LockControls() override;
	virtual void UnLockControls() override;

	AController* LastUnpossessedController = nullptr;

public:
	void BroadCastDatas();

public:
	//-----DELEGATE-----
	FAIFloatChangedController OnHealthChanged;
	FAIFloatChangedController OnMaxHealthChanged;
	FAIFloatChangedController OnShieldChanged;
	FAIFloatChangedController OnMaxShieldChanged;
protected:

	virtual void BeginPlay() override;

	virtual FVector lookingAtPosition() override;

	FVector LookingAtPosition;

//////////////// Teamable
public:
	virtual ASoldierTeam* GetTeam() override;
	virtual bool SetTeam(ASoldierTeam* _Team) override;

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