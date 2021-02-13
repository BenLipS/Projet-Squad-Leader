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
	bool isNextFireReady;

	// TODO: Use cooldown from ability system
	FTimerHandle timerReloadNextShoot;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Stats")
	float timeToReloadNextShoot;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Stats")
	bool isAutomatic;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Stats")
	int penetration;

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
	virtual void tryFiring();
	void tryFiring(const FGameplayEffectSpecHandle _damageEffectSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Fire")
	virtual void fire();

	virtual void BeginPlay() override;
	virtual void onReadyToShoot();
};