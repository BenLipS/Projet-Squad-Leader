#pragma once

#include "CoreMinimal.h"
#include "../Soldier.h"
#include "../../AI/Mission.h"
#include "SoldierAI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAIFloatChangedController, float, newValue, AAISquadController*, controller);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIFloatChanged, float, newValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIStateChanged, AIBasicState, newValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIClassChanged, SoldierClass, newValue);

//For client knowledge purpose
USTRUCT()
struct FSoldierAIData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Health;

	UPROPERTY()
	float MaxHealth;

	UPROPERTY()
	float Shield;

	UPROPERTY()
	float MaxShield;

	UPROPERTY()
	AIBasicState MissionState;

	UPROPERTY()
	SoldierClass ClassSoldier = SoldierClass::NONE;

	void OnHealthChanged(float newHealth);
	void OnMaxHealthChanged(float newMaxHealth);
	void OnShieldChanged(float newShield);
	void OnMaxShieldChanged(float newMaxShield);
	void OnStateChanged(AIBasicState newState);
	void OnClassChanged(SoldierClass newState);

	FAIFloatChanged OnHealthNotify;
	FAIFloatChanged OnMaxHealthNotify;
	FAIFloatChanged OnShieldNotify;
	FAIFloatChanged OnMaxShieldNotify;
	FAIStateChanged OnStateNotify;
	FAIClassChanged OnClassNotify;

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

	FVector LookingAtPosition;

public:
	virtual FVector GetLookingAtPosition(const float _MaxRange = 99999.f) const override;


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

	// LaunchGrenade
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilityLaunchGrenade();

	// LaunchHeal
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilityLaunchHeal();

	// LaunchShield
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilityLaunchShield();

	// LaunchShield
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilityRegenShield();

	// LaunchShield
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilityLaunchMine();

	// LaunchShield
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilityOverHeat();

	UFUNCTION(BlueprintCallable, Category = "Shoot")
	void SetLookingAtPosition(const FVector& _LookingAtPosition);

	virtual void Die() override;

	void InitializeAttributeChangeCallbacks() override;

	void OnBlurredVisionFromJammer(const bool _IsBlurred) override;

	virtual SoldierClass GetClass() override;

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
	virtual void OnReceiveDamage(const FVector& _ImpactPoint, const FVector& _SourcePoint) override;
	virtual FVector GetRespawnPoint() override;
};