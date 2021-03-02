#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "AbilitySystemInterface.h"
#include "../AbilitySystem/Soldiers/AbilitySystemSoldier.h"
#include "Weapon.generated.h"

UCLASS(Abstract)
class SQUADLEADER_API AWeapon : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	AWeapon();

//////////////// Ability System
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System Component", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemSoldier* AbilitySystemComponent;

public:
	void InitializeAbilitySystemComponent(UAbilitySystemSoldier* _abilitySystemComponent);
	UAbilitySystemSoldier* GetAbilitySystemComponent() const;

//////////////// Fire
protected:
	bool IsNextFireReady;

	// TODO: Use cooldown from ability system
	FTimerHandle TimerReloadNextShoot;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Stats")
	float TimeToReloadNextShoot;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Stats")
	bool IsAutomatic;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Stats")
	int Penetration;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Stats")
	float FieldOfViewAim;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetFieldOfViewAim() const;

protected:
	UPROPERTY(BluePrintReadWrite, EditAnywhere, Category = "Stats")
	float Damage;

	// Damage impact
	UPROPERTY(BlueprintReadWrite, Category = "Fire | Impact")
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UFUNCTION(BlueprintCallable, Category = "Fire | Impact")
	virtual void ApplyImpactDamage(UAbilitySystemComponent* _targetASC);

	// Additional impact effects
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fire | Impact")
	TArray<TSubclassOf<class UGameplayEffect>> ImpactEffects;

	UFUNCTION(BlueprintCallable, Category = "Fire | Impact")
	virtual void ApplyImpactEffects(UAbilitySystemComponent* _targetASC);

public:
	UFUNCTION(BlueprintCallable, Category = "Fire")
	virtual void TryFiring();
	void TryFiring(const FGameplayEffectSpecHandle _damageEffectSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Fire")
	virtual void Fire();

	virtual void BeginPlay() override;
	virtual void OnReadyToShoot();
};