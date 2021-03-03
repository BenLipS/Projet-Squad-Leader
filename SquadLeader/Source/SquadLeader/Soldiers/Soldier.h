#pragma once

#include "Core.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "camera/cameracomponent.h"
#include "AbilitySystemInterface.h"
#include "../AbilitySystem/Soldiers/AttributeSetSoldier.h"
#include "../AbilitySystem/Soldiers/AbilitySystemSoldier.h"
#include "../Weapons/Weapon.h"
#include "SoldierTeam.h"
#include "Net/UnrealNetwork.h"
#include "Soldier.generated.h"


UCLASS()
class SQUADLEADER_API ASoldier : public ACharacter, public IAbilitySystemInterface
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

	// Abilities
	static FGameplayTag SkillRunTag;
	static FGameplayTag SkillJumpTag;
	static FGameplayTag SkillCrouchTag;
	static FGameplayTag SkillFireWeaponTag;
	static FGameplayTag SkillGrenadeTag;
	static FGameplayTag SkillAimTag;
	static FGameplayTag SkillAreaEffectFromSelfTag;

protected:
	virtual void DeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void RunningTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void JumpingTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void FightingTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void AimingTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

//////////////// Attributes
public:
	// Getters
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	int32 GetCharacterLevel() const;

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
	void LookUp(const float _Val);

	UFUNCTION()
	void Turn(const float _Val);

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

	void addToInventory(AWeapon* _weapon);

	void SetCurrentWeapon(class AWeapon* _newWeapon, class AWeapon* _previousWeapon = nullptr);

	UFUNCTION()
	void OnRep_CurrentWeapon(class AWeapon* _lastWeapon);

public:
	AWeapon* getCurrentWeapon() const noexcept { return currentWeapon; }
	////////////////  PlayerTeam
	// Appel du c�t� serveur pour actualiser l'�tat du rep�re 
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerChangeTeam(TSubclassOf<ASoldierTeam> _PlayerTeam);

	UFUNCTION() // Doit toujours �tre UFUNCTION() quand il s'agit d'une fonction �OnRep notify�
		void OnRep_ChangeTeam();

	UPROPERTY(EditAnywhere, BluePrintReadWrite, ReplicatedUsing = OnRep_ChangeTeam, Category = "PlayerTeam")
		TSubclassOf<ASoldierTeam> PlayerTeam;
	TSubclassOf<ASoldierTeam> OldPlayerTeam;  // Local buffer used for team change
	
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerCycleBetweenTeam();

	// Connected to the "L" key
	void cycleBetweenTeam();


public:
	/////////////// Respawn
	UFUNCTION()
	virtual FVector GetRespawnPoint() { return FVector(0.f, 0.f, 1500.f); }  // function overide in SoldierPlayer and Soldier AI


	//For AIPerception
private:
	class UAIPerceptionStimuliSourceComponent* stimulus;

	void setup_stimulus();

};
