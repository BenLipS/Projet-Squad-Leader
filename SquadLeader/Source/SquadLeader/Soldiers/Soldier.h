#pragma once

#include "Core.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "camera/cameracomponent.h"
#include "AbilitySystemInterface.h"
#include "../AbilitySystem/Soldiers/AttributeSetSoldier.h"
#include "../AbilitySystem/Soldiers/AbilitySystemSoldier.h"
#include "SquadLeader/SquadLeader.h"
#include "Interface/Teamable.h"
//
#include "SoldierTeam.h"
//
#include "Net/UnrealNetwork.h"
#include "Soldier.generated.h"

class ASL_Weapon;
class UGameplayAbilitySoldier;
class UGameplayEffect;
class UGE_UpdateStats;
class UMatineeCameraShake;

UENUM()
enum class SoldierClass : uint8 {
	NONE,
	ASSAULT,
	HEAVY,
	SUPPORT
};

USTRUCT()
struct SQUADLEADER_API FSoldier_Inventory
{
	GENERATED_USTRUCT_BODY()

	FSoldier_Inventory() = default;

	UPROPERTY()
	TArray<ASL_Weapon*> Weapons;

	// Consumable items ?
	// Grenade ?
};

UCLASS()
class SQUADLEADER_API ASoldier : public ACharacter, public IAbilitySystemInterface, public ITeamable
{
	GENERATED_BODY()

public:
	ASoldier(const FObjectInitializer& _ObjectInitializer);
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

//////////////// Inits
protected:
	void InitCameras();
	void InitMeshes();
	void InitMovements();

//////////////// Resets
	void ResetWeapons();

//////////////// Controllers
protected:
	// Lock any interraction with this soldier
	UFUNCTION()
	virtual void LockControls();

	// Unlock interractions with this soldier
	UFUNCTION()
	virtual void UnLockControls();

//////////////// Ability System
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System Component", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemSoldier* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System Component", meta = (AllowPrivateAccess = "true"))
	UAttributeSetSoldier* AttributeSet;

public:
	UAbilitySystemSoldier* GetAbilitySystemComponent() const override;
	UAttributeSetSoldier* GetAttributeSet() const;

protected:
	// Define the start abilities - excluding the main fighting abilities
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ability System Component|Abilities")
	TArray<TSubclassOf<UGameplayAbilitySoldier>> StartupAbilities;

	// Ability1
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ability System Component|Abilities")
	TSubclassOf<UGameplayAbilitySoldier> Ability1;

	// Ability2
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ability System Component|Abilities")
	TSubclassOf<UGameplayAbilitySoldier> Ability2;

	// Ability3
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ability System Component|Abilities")
	TSubclassOf<UGameplayAbilitySoldier> Ability3;

public:
	UFUNCTION()
	TSubclassOf<UGameplayAbilitySoldier> GetClassAbility1() const;

	UFUNCTION()
	TSubclassOf<UGameplayAbilitySoldier> GetClassAbility2() const;

	UFUNCTION()
	TSubclassOf<UGameplayAbilitySoldier> GetClassAbility3() const;

	// Additional effect (like hp regen)
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability System Component|Effects")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	// Define the stats for any level. It is call at the beginning and when leveling up
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ability System Component|Attributes")
	TSubclassOf<UGE_UpdateStats> StatAttributeEffects;

	UPROPERTY()
	bool bAbilitiesInitialized;	
	virtual void InitializeAttributes();
	void InitializeAbilities();
	void InitializeStartupAbilities();
	virtual void InitializeMainFightingAbilities();
	void AddStartupEffects();
	void InitializeTagChangeCallbacks();
	virtual void InitializeAttributeChangeCallbacks();

public:
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilities(const FGameplayTagContainer& _TagContainer);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbility(const FGameplayTag& _Tag);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbilities(const FGameplayTagContainer& _TagContainer);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbility(const FGameplayTag& _Tag);

public:
	TSubclassOf<UGE_UpdateStats> GetStatAttributeEffects() const;

//////////////// Cooldowns
public:
	float GetCooldownRemainingFromAbilityID(const ESoldierAbilityInputID _AbilityID);
	bool IsInCooldown(const FGameplayTag& _Tag);

//////////////// Tag Change Callbacks
public:
	// Weapon
	FGameplayTag CurrentWeaponTag;

protected:
	virtual void DeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void BlurredFromJammerTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

public:
	UFUNCTION()
	virtual void OnBlurredVisionFromJammer(const bool _IsBlurred);

//////////////// Attributes
public:
	UPROPERTY(BluePrintReadWrite, Category = "Attributes")
	uint8 InfluenceRadius = 2;

	UPROPERTY(BluePrintReadWrite, Category = "Attributes")
	float InfluenceWeight = 0.5f;

	// Getters
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetEXP() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetEXPLevelUp() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetRemainEXPForLevelUp() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void GrantEXP(const float _EXP);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetShield() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxShield() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMoveSpeedWalk() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMoveSpeedCrouch() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMoveSpeedMultiplier() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	FVector GetLocation() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	uint8 GetInfluenceRadius() const noexcept;


	// Class Name:
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual SoldierClass GetClass() { return SoldierClass::NONE; }

	UPROPERTY(EditDefaultsOnly, Category = "Player class")
	FString ClassName = "Soldier";

	// Attribute changed callbacks
	FDelegateHandle HealthChangedDelegateHandle;
	virtual void HealthChanged(const FOnAttributeChangeData& _Data);

	virtual void LevelUp();
	virtual void Die();
	virtual void Respawn();
	virtual void OnReceiveDamage(const FVector& _ImpactPoint, const FVector& _SourcePoint);

//////////////// Cameras
public:
	void SetToFirstCameraPerson();
	void SetToThirdCameraPerson();

// TODO: Change to protected and use getters/setters
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CurrentCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* ThirdPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComponent;

// Camera Shake
protected:
	// Camera shake when receiving damage from a fire weapon
	UPROPERTY(EditDefaultsOnly, Category = "Camera|Camera Shake")
	TSubclassOf<UMatineeCameraShake> CameraShakeReceiveDamageClass;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera|Camera Shake")
	TSubclassOf<UMatineeCameraShake> GetCameraShakeReceiveDamageClass() const;

	// Return camera shake fire class of the current weapon if exists
	UFUNCTION(BlueprintCallable, Category = "Camera|Camera Shake")
	TSubclassOf<UMatineeCameraShake> GetCameraShakeFireClass() const;

	UFUNCTION(BlueprintCallable)
	void ShakeCamera(TSubclassOf<UMatineeCameraShake> _CameraShakeClass);

	//LevelUp Animation
	UFUNCTION(BlueprintImplementableEvent)
	void LevelUpAnimation();

protected:
	UPROPERTY(BlueprintReadOnly, Replicated)
	FRotator SyncControlRotation;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera")
	FRotator GetSyncControlRotation() const noexcept;

protected:
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSyncControlRotation(const FRotator& _Rotation);
	void ServerSyncControlRotation_Implementation(const FRotator& _Rotation);
	bool ServerSyncControlRotation_Validate(const FRotator& _Rotation);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastSyncControlRotation(const FRotator& _Rotation);
	void MulticastSyncControlRotation_Implementation(const FRotator& _Rotation);
	bool MulticastSyncControlRotation_Validate(const FRotator& _Rotation);

protected:
	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Camera")
	bool bIsFirstPerson;

public:
	UFUNCTION()
	void onSwitchCamera();

////////////////  Meshes
public:
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* FirstPersonMesh;

	// Relative transform of the mesh in capsule component - Cache value from BeginPlay. It is used for ragdoll
	UPROPERTY(BlueprintReadOnly, Category = "Mesh")
	FTransform CacheRelativeTransformMeshInCapsule;

	// Default right hand attach point if the weapon does not give one
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Weapon")
	FName WeaponAttachPointRightHand;

	// Default left hand attach point if the weapon does not give one
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Weapon")
	FName WeaponAttachPointLeftHand;

	UFUNCTION()
	void StartRagdoll();

	UFUNCTION()
	void StopRagdoll();

//////////////// Movement
public:
// Move direction
	UFUNCTION()
	void MoveForward(const float _Val);

	UFUNCTION()
	void MoveRight(const float _Val);

// Looking direction
	UFUNCTION()
	virtual void LookUp(const float _Val);

	UFUNCTION()
	virtual void Turn(const float _Val);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual FVector GetLookingAtPosition(const float _MaxRange = 99999.f) const;

	// Run
	UFUNCTION(BlueprintCallable, Category = "Movement|Run")
	bool StartRunning();

	UFUNCTION(BlueprintCallable, Category = "Movement|Run")
	void StopRunning();

	UFUNCTION(BlueprintCallable, Category = "Movement|Run")
	bool IsRunning() const noexcept;

protected:
	bool bIsRunning = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool Walk();

	virtual void Landed(const FHitResult& _Hit) override;

// Field of view
public:
	UFUNCTION(BlueprintCallable, Category = "Movement|Aim")
	void StartAiming();

	UFUNCTION(BlueprintCallable, Category = "Movement|Aim")
	void StopAiming();

	UFUNCTION(BlueprintCallable, Category = "Movement|Aim")
	bool IsAiming() const noexcept;

protected:
	bool bIsAiming = false;

	// Normal FOV in idle or walking mode
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera|FOV")
	float BaseFOVNormal = 90.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera|FOV")
	float BaseFOVRunning = 100;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera|FOV")
	void UpdateFOV();

private:
	// FOV after checking soldier mode (running state etc...)
	float CurrentFOV;

	void ZoomInFOV();
	void ZoomOutFOV();
	void FinishFOVAnimation();

	FTimerHandle TimerFOVAnimation;

protected:
	// Time to realize an entire FOV animation
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera|FOV")
	float TimeFOVAnimation = 0.15f;

	// Time between each FOV change
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|FOV", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float TimeBetweenFOVChange = 0.01f;

private:
	// Value to add to FOV for animation. This is calculated with TimeFOVAnimation and TimeBetweenFOVChange
	float ZoomFOVAdd;

//////////////// Inventory
protected:
	void SpawnDefaultInventory();
	
	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	FSoldier_Inventory Inventory;

	UFUNCTION()
	void OnRep_Inventory();

//////////////// Weapons
public:
	void OnStartFiring();
	void OnStopFiring();

protected:
	bool bIsFiring = false;

public:
	bool IsFiring() const noexcept;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory|Weapon")
	TArray<TSubclassOf<ASL_Weapon>> DefaultInventoryWeaponClasses;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	ASL_Weapon* CurrentWeapon;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	ASL_Weapon* GetCurrentWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	TArray<ASL_Weapon*> GetAllWeapons() const;

	// Define the attach point for the mesh as right hand - This is the default point - this will be useful to handle animations
	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	void UseCurrentWeaponWithRightHand();

	// Define the attach point for the mesh as left hand - this will be useful to handle animations
	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	void UseCurrentWeaponWithLeftHand();

	bool bChangedWeaponLocally;

	// Adds a new weapon to the inventory.
	// Returns false if the weapon already exists in the inventory, true if it's a new weapon.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	bool AddWeaponToInventory(ASL_Weapon* _NewWeapon, const bool _bEquipWeapon = false);

	//UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	void EquipWeapon(ASL_Weapon* _NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	void EquipWeapon(UClass* _WeaponClass);

	UFUNCTION(Server, Reliable, Category = "Inventory|Weapon")
	void ServerEquipWeapon(ASL_Weapon* _NewWeapon);
	bool ServerEquipWeapon_Validate(ASL_Weapon* _NewWeapon);
	void ServerEquipWeapon_Implementation(ASL_Weapon* _NewWeapon);

protected:
	// Return true if a weapon with the same class already exists
	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	bool DoesWeaponExistInInventory(ASL_Weapon* _Weapon);

	// Return the weapon in inventoery that matches the given weapon. Return nullptr if there is no such weapon
	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	ASL_Weapon* GetWeaponWithSameClassInInventory(ASL_Weapon* _Weapon);

	void SetCurrentWeapon(ASL_Weapon* _NewWeapon, ASL_Weapon* _LastWeapon);

	// Unequips the specified weapon. Used when OnRep_CurrentWeapon fires.
	void UnEquipWeapon(ASL_Weapon* WeaponToUnEquip);

	UFUNCTION()
	void OnRep_CurrentWeapon(ASL_Weapon* _LastWeapon);

	// The CurrentWeapon is only automatically replicated to simulated clients.
	// The autonomous client can use this to request the proper CurrentWeapon from the server when it knows it may be
	// out of sync with it from predictive client-side changes.
	UFUNCTION(Server, Reliable, Category = "Inventory|Weapon")
	void ServerSyncCurrentWeapon();
	void ServerSyncCurrentWeapon_Implementation();
	bool ServerSyncCurrentWeapon_Validate();

	// The CurrentWeapon is only automatically replicated to simulated clients.
	// Use this function to manually sync the autonomous client's CurrentWeapon when we're ready to.
	// This allows us to predict weapon changes (changing weapons fast multiple times in a row so that the server doesn't replicate and clobber our CurrentWeapon).
	UFUNCTION(Client, Reliable, Category = "Inventory|Weapon")
	void ClientSyncCurrentWeapon(ASL_Weapon* _InWeapon);
	void ClientSyncCurrentWeapon_Implementation(ASL_Weapon* _InWeapon);
	bool ClientSyncCurrentWeapon_Validate(ASL_Weapon* _InWeapon);

//////////////// Soldier team
public:
	UFUNCTION()
	void RefreshTeam();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCycleBetweenTeam();

	// Connected to the "L" key
	virtual void cycleBetweenTeam();

//////////////// Teamable
protected:
	UPROPERTY(Replicated)
	ASoldierTeam* Team;

public:
	virtual ASoldierTeam* GetTeam() override;
	virtual bool SetTeam(ASoldierTeam* _Team) override;

/////////////// Respawn
public:
	UFUNCTION()
	virtual FVector GetRespawnPoint() { return FVector(0.f, 0.f, 1500.f); }  // function overide in SoldierPlayer and Soldier AI

//////////////// For AIPerception
private:
	class UAIPerceptionStimuliSourceComponent* stimulus;

	void setup_stimulus();

//////////////// Particles
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	UParticleSystem* LevelUpFX;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	FVector LevelUpFXRelativeLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	FRotator LevelUpFXRotator;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	FVector LevelUpFXScale;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	UParticleSystem* ImpactHitFX;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	FVector ImpactHitFXScale;

	UFUNCTION(Reliable, Client, WithValidation)
	void ClientSpawnLevelUpParticle();
	void ClientSpawnLevelUpParticle_Implementation();
	bool ClientSpawnLevelUpParticle_Validate();

//////////////// Montages
public:
	// Level start
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages")
	UAnimMontage* StartGameMontage;

	// Death
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages")
	UAnimMontage* DeathMontage;

	// Respawn
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages")
	UAnimMontage* RespawnMontage;

	// Throw projectile
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages")
	UAnimMontage* ThrowProjectileMontage;

	// Cast Spell - For miscellaneous abilities
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages")
	UAnimMontage* CastSpellMontage;

	// Hit react - Front
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages|Hit React")
	UAnimMontage* HitReactFrontMontage;

	// Hit react - Back
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages|Hit React")
	UAnimMontage* HitReactBackMontage;

	// Hit react - Right
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages|Hit React")
	UAnimMontage* HitReactRightMontage;

	// Hit react - Left
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation|Montages|Hit React")
	UAnimMontage* HitReactLeftMontage;

protected:
	void HandleDeathMontage();

	// Callbacks
	FOnMontageEnded StartGame_SoldierMontageEndedDelegate;
	FOnMontageEnded Respawn_SoldierMontageEndedDelegate;

	UFUNCTION()
	virtual void OnStartGameMontageCompleted(UAnimMontage* _Montage, bool _bInterrupted);

	UFUNCTION()
	virtual void OnRespawnMontageCompleted(UAnimMontage* _Montage, bool _bInterrupted);

public:
	UFUNCTION()
	void ShowImpactHitEffect();

	// Projectile forwardVector to launch from
	UFUNCTION()
	virtual FVector GetLookingDirection();

protected:
	UPROPERTY()
		FVector LastPosition; 

	UPROPERTY()
		FVector CurrentPosition;
};