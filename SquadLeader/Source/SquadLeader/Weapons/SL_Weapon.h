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
	virtual void Destroyed() override;

public:
	virtual void PreReplication(IRepChangedPropertyTracker& _ChangedPropertyTracker) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	virtual void ResetWeapon();

	// For the HUD
	void ForceUpdateAmmo();

//////////////// Mesh
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* WeaponMesh;

public:
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetWeaponMesh() const;

// Attach points
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FName MuzzleAttachPoint = FName{ "Muzzle" };

	// Default right hand attach point a character could use - 
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FName RightHandAttachPoint;

	// Default left hand attach point if the weapon does not give one
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FName LeftHandAttachPoint;

public:
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	FName GetMuzzleAttachPoint() const;

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	FName GetRightHandAttachPoint() const;

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	FName GetLeftHandAttachPoint() const;

//////////////// Owning Soldier

protected:
	UPROPERTY(BlueprintReadOnly, Replicated)
	ASoldier* OwningSoldier;

public:
	void SetOwningSoldier(ASoldier* _InOwningCharacter);

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

protected:
	// Cache tags
	FGameplayTag WeaponAbilityTag;
	FGameplayTag WeaponIsFiringTag;

//////////////// Stats
protected:
// Damage
	UPROPERTY(BluePrintReadWrite, EditAnywhere, Category = "Stats|Damage")
	FScalableFloat Damage = FScalableFloat{ 1.f };

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats|Damage")
	float HeadShotMultiplier = 2.5f;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats|Damage")
	float GetWeaponDamage() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Damage")
	void SetWeaponDamage(const float _Damage) noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Damage")
	float GetHeadShotMultiplier() const noexcept;

// Range
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
	float MaxRange = 999999.f;

public:
	float GetMaxRange() const noexcept;

// Aim
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
	float FieldOfViewAim = 50.f;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetFieldOfViewAim() const noexcept;

// Weight
protected:
	// Multiplier to influence owner move speed
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats|Weight", meta = (ClampMax = "1.0"))
	float MoveSpeedMultiplier = 1.f;

	// If true the owner should not be able to jump or crouch
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats|Weight")
	bool IsHeavy = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	float GetMoveSpeedMultiplier() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	bool IsHeavyWeapon() const noexcept;

// Ammo
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated, Category = "Stats|Ammo")
	float TimeBetweenShots = 0.1f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated, Category = "Stats|Ammo")
	int32 CurrentAmmo = 50;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated, Category = "Stats|Ammo")
	int32 MaxAmmo = 50 ;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stats|Ammo")
	bool bInfiniteAmmo = false;

	// Wheter the weapon can be reloaded right after reaching 0 ammo
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stats|Ammo")
	bool bReloadIfOutOfAmmo = true;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	float GetTimeBetweenShots() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	void SetTimeBetweenShots(const float _Time) noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	int32 GetCurrentAmmo() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	int32 GetMaxAmmo() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	bool IsFullAmmo() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	void SetCurrentAmmo(const int32 _NewAmmo);

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	void DecrementAmmo();

	UFUNCTION(BlueprintImplementableEvent)
	void OnOutOfAmmo();

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	void SetMaxAmmo(const int32 _NewMaxAmmo);

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	bool HasAmmo() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	void ReloadWeapon();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerReloadWeapon();
	void ServerReloadWeapon_Implementation();
	bool ServerReloadWeapon_Validate();

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	void SetHasInfiniteAmmo(const bool _hasInfiniteAmmo);

	UFUNCTION(BlueprintCallable, Category = "Stats|Ammo")
	bool HasInfiniteAmmo() const noexcept;

// Accuracy
protected:
	// Constant spread on fire. It defines the angle of the cone to randomly generate a fire
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats|Accuracy")
	float BaseSpread = 0.f;

	// Multiplier of the final spread when aiming
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats|Accuracy")
	float AimingSpreadMod = 1.f;

	// Increment this value to the final spread after a fire. This is accumulated for automatic weapon
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats|Accuracy")
	float TargetingSpreadIncrement = 0.f;

	// Max increase of the spread.This works with TargetingSpreadIncrement
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats|Accuracy")
	float TargetingSpreadMax = 0.f;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats|Accuracy")
	float GetBaseSpread() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Accuracy")
	float GetAimingSpreadMod() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Accuracy")
	float GetTargetingSpreadIncrement() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Stats|Accuracy")
	float GetTargetingSpreadMax() const noexcept;

//////////////// Animations
public:
	// Fire
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages")
	UAnimMontage* FireMontage;

	// Reload
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages")
	UAnimMontage* ReloadMontage;

//////////////// Collision
public:
	// Collision profile - See Preset in Engine - Collision 
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Collision")
	FName CollisionProfileName = FName{ "InstantWeaponFire" };

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Collision")
	bool bDebugTrace = true;

//////////////// Traces
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
