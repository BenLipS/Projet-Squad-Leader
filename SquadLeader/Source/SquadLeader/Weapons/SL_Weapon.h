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
	UPROPERTY()
	UAbilitySystemSoldier* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, Category = "Ability System")
	TArray<TSubclassOf<UGameplayAbilitySoldier>> Abilities;

	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void AddAbilities();
	virtual void RemoveAbilities();
	virtual int32 GetAbilityLevel(ESoldierAbilityInputID _AbilityID);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability System")
	FGameplayTag DefaultFireMode;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Ability System")
	FGameplayTag FireMode;

	// Cache tags
	FGameplayTag WeaponAbilityTag;
	FGameplayTag WeaponIsFiringTag;

//////////////// Ammo
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_Ammo, Category = "Stats | Ammo")
	int32 CurrentAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_MaxAmmo, Category = "Stats | Ammo")
	int32 MaxAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stats | Ammo")
	bool bInfiniteAmmo;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats | Ammo")
	virtual int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Stats | Ammo")
	virtual int32 GetMaxAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Stats | Ammo")
	bool IsFullAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Stats | Ammo")
	virtual void SetAmmo(const int32 _NewAmmo);

	UFUNCTION(BlueprintCallable, Category = "Stats | Ammo")
	virtual void SetMaxAmmo(const int32 _NewMaxAmmo);

	UFUNCTION(BlueprintCallable, Category = "Stats | Ammo")
	virtual bool HasInfiniteAmmo() const;

	UFUNCTION()
	virtual void OnRep_Ammo(int32 _OldPrimaryClipAmmo);

	UFUNCTION()
	virtual void OnRep_MaxAmmo(int32 _OldMaxPrimaryClipAmmo);

//////////////// Traces
protected:
	UPROPERTY()
	ASL_LineTrace* LineTraceTargetActor;

	UPROPERTY()
	ASL_SphereTrace* SphereTraceTargetActor;

public:
	// Getter for LineTraceTargetActor. Spawns it if it doesn't exist yet
	UFUNCTION(BlueprintCallable, Category = "Collision | Trace")
	ASL_LineTrace* GetLineTraceTargetActor();

	// Getter for SphereTraceTargetActor. Spawns it if it doesn't exist yet
	UFUNCTION(BlueprintCallable, Category = "Collision | Trace")
	ASL_SphereTrace* GetSphereTraceTargetActor();
};
