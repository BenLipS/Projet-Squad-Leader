#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponLaser.generated.h"

UCLASS()
class SQUADLEADER_API AWeaponLaser : public AWeapon
{
	GENERATED_BODY()

public:
	AWeaponLaser();
	void fire() override;

protected:
	virtual void BeginPlay() override;

//////////////// Fire
	TArray<FHitResult> GetActorsFromLineTrace(const FVector& _startLocation, const FVector& _endLocation);

	// Damage impact
	virtual void ApplyImpactDamage(UAbilitySystemComponent* _targetASC) override;

	// Additional impact effects
	virtual void ApplyImpactEffects(UAbilitySystemComponent* _targetASC) override;
};