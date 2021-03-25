#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquadLeader/SquadLeader.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "SL_Weapon.generated.h"

class ASL_LineTrace;
class ASL_SphereTrace;
class ASoldier;
class UAnimMontage;
class UAbilitySystemSoldier;
class UGameplayAbilitySoldier;

UCLASS()
class SQUADLEADER_API ASL_Weapon : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	ASL_Weapon();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ability System")
	FGameplayTag WeaponTag;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type _EndPlayReason) override;

public:
	virtual void PreReplication(IRepChangedPropertyTracker& _ChangedPropertyTracker) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(BlueprintReadOnly, Replicated)
	ASoldier* OwningSoldier;

public:
	void SetOwningCharacter(ASoldier* _InOwningCharacter);
	
	UFUNCTION(BlueprintCallable)
	virtual void ResetWeapon();

//////////////// Ability System
protected:
	// AbilitySystemComponent of the soldier owner - null if it doesn't have an owner
	UPROPERTY()
	UAbilitySystemSoldier* AbilitySystemComponent;

	// Granted abilities to the owner
	UPROPERTY(EditAnywhere, Category = "Ability System")
	TArray<TSubclassOf<UGameplayAbilitySoldier>> Abilities;

	// Cache to remove granted abilities if the weapon changes owner
	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void AddAbilities();
	virtual void RemoveAbilities();
	virtual int32 GetAbilityLevel(ESoldierAbilityInputID _AbilityID);

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ability System")
	FGameplayTag FireMode;

public:
	FGameplayTag GetFireMode() const noexcept;

	static FGameplayTag FireModeAutomaticTag;
	static FGameplayTag FireModeSemiAutoTag;

protected:
	// Cache tags
	FGameplayTag WeaponAbilityTag;
	FGameplayTag WeaponIsFiringTag;

//////////////// Stats
protected:
// Damage
	UPROPERTY(BluePrintReadWrite, EditAnywhere, Category = "Stats|Damage")
	float Damage;

// Aim
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
	float FieldOfViewAim;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	float GetWeaponDamage() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetFieldOfViewAim() const noexcept;

// Ammo
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated, Category = "Stats|Ammo")
	float TimeBetweenShots;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_Ammo, Category = "Stats|Ammo")
	int32 CurrentAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_MaxAmmo, Category = "Stats|Ammo")
	int32 MaxAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stats|Ammo")
	bool bInfiniteAmmo;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	float GetTimeBetweenShots() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	int32 GetCurrentAmmo() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	int32 GetMaxAmmo() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	bool IsFullAmmo() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	virtual void SetAmmo(const int32 _NewAmmo);

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	virtual void SetMaxAmmo(const int32 _NewMaxAmmo);

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	bool HasInfiniteAmmo() const;

	UFUNCTION()
	virtual void OnRep_Ammo(int32 _OldPrimaryClipAmmo);

	UFUNCTION()
	virtual void OnRep_MaxAmmo(int32 _OldMaxPrimaryClipAmmo);

//////////////// Traces
	// TODO: Only have one generic trace that will be set from blueprint ?
protected:
	UPROPERTY()
	ASL_LineTrace* LineTraceTargetActor;

	UPROPERTY()
	ASL_SphereTrace* SphereTraceTargetActor;

public:
	// Getter for LineTraceTargetActor. Spawns it if it doesn't exist yet
	UFUNCTION(BlueprintCallable, Category = "Collision|Trace")
	ASL_LineTrace* GetLineTraceTargetActor();

	// Getter for SphereTraceTargetActor. Spawns it if it doesn't exist yet
	UFUNCTION(BlueprintCallable, Category = "Collision|Trace")
	ASL_SphereTrace* GetSphereTraceTargetActor();
};
