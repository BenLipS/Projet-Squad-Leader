#pragma once

#include "Core.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "camera/cameracomponent.h"
#include "AbilitySystemInterface.h"
#include "../AbilitySystem/Soldiers/AttributeSetSoldier.h"
#include "../AbilitySystem/Soldiers/AbilitySystemSoldier.h"
#include "../Weapons/Weapon.h"
#include "Interface/Teamable.h"
//
#include "SoldierTeam.h"
//
#include "Net/UnrealNetwork.h"
#include "Soldier.generated.h"


UCLASS()
class SQUADLEADER_API ASoldier : public ACharacter, public IAbilitySystemInterface, public ITeamable
{
	GENERATED_BODY()

public:
	ASoldier(const FObjectInitializer& _ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;


//////////////// Inits
protected:
	void initCameras();
	void initMeshes();
	void initMovements();
	virtual void initWeapons();

//////////////// Ability System
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System Component", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemSoldier* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute Set", meta = (AllowPrivateAccess = "true"))
	UAttributeSetSoldier* AttributeSet;

public:
	UAbilitySystemSoldier* GetAbilitySystemComponent() const override;
	UAttributeSetSoldier* GetAttributeSet() const;

protected:
	// Define the default stats
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffects;

	// Define the default abilities
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbilitySoldier>> CharacterDefaultAbilities;

	// Additional applied effect (for instance hp regen)
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	UPROPERTY()
	bool bAbilitiesInitialized;	
	virtual void InitializeAttributes();
	void InitializeAbilities();
	void AddStartupEffects();
	void InitializeTagChangeCallbacks();
	void InitializeAttributeChangeCallbacks();

//////////////// Tag Change Callbacks
public:
	// States
	static FGameplayTag StateDeadTag;
	static FGameplayTag StateRunningTag;
	static FGameplayTag StateJumpingTag;
	static FGameplayTag StateCrouchingTag;
	static FGameplayTag StateFightingTag;
	static FGameplayTag StateAimingTag;
	static FGameplayTag StateGivingOrderTag;

	// Abilities
	static FGameplayTag SkillRunTag;
	static FGameplayTag SkillJumpTag;
	static FGameplayTag SkillCrouchTag;
	static FGameplayTag SkillFireWeaponTag;
	static FGameplayTag SkillGrenadeTag;
	static FGameplayTag SkillAimTag;
	static FGameplayTag SkillAreaEffectFromSelfTag;
	static FGameplayTag SkillGiveOrderTag;

protected:
	virtual void DeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void RunningTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void JumpingTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void FightingTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void AimingTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void GivingOrderTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

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
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;

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

	// Attribute changed callbacks
	FDelegateHandle HealthChangedDelegateHandle;
	virtual void HealthChanged(const FOnAttributeChangeData& _Data);

	virtual void Die();
	virtual void Respawn();

//////////////// Cameras
public:
	void setToFirstCameraPerson();
	void setToThirdCameraPerson();

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

protected:
	UPROPERTY(VisibleAnywhere)
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

//////////////// Movement
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
	virtual FVector lookingAtPosition();

	// Run
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool StartRunning();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopRunning();
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool Walk();

//////////////// Weapons
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool wantsToFire = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetWantsToFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWantsToFire(const bool _want);

	void SetWantsToFire(const bool _want, const FGameplayEffectSpecHandle _damageEffectSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartAiming();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopAiming();

protected:
	bool bDefaultWeaponsInitialized;

	// Default inventory
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<TSubclassOf<class AWeapon>> DefaultWeaponClasses;

	// Current inventory
	UPROPERTY(Transient, Replicated)
	TArray<class AWeapon*> Inventory;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	AWeapon* currentWeapon;

	void AddToInventory(AWeapon* _Weapon);

	void SetCurrentWeapon(class AWeapon* _NewWeapon, class AWeapon* _PreviousWeapon = nullptr);

	UFUNCTION()
	void OnRep_CurrentWeapon(class AWeapon* _LastWeapon);

public:
	AWeapon* getCurrentWeapon() const noexcept { return currentWeapon; }

	//////////////// Soldier team
	UPROPERTY(EditAnywhere, Category = "PlayerTeam")
		TSubclassOf<ASoldierTeam> InitialTeam;  // for debug use

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerCycleBetweenTeam();

	// Connected to the "L" key
	virtual void cycleBetweenTeam();
	
	//////////////// Teamable
	virtual TSubclassOf<ASoldierTeam> GetTeam() override { return nullptr; };  // function overide in SoldierPlayer and Soldier AI
	virtual bool SetTeam(TSubclassOf<ASoldierTeam> _Team) override { return false; };  // function overide in SoldierPlayer and Soldier AI


public:
	/////////////// Respawn
	UFUNCTION()
	virtual FVector GetRespawnPoint() { return FVector(0.f, 0.f, 1500.f); }  // function overide in SoldierPlayer and Soldier AI


	//For AIPerception
private:
	class UAIPerceptionStimuliSourceComponent* stimulus;

	void setup_stimulus();

};
