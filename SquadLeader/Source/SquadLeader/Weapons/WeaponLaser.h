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
	void Fire() override;

protected:
	virtual void BeginPlay() override;

//////////////// Fire
	TArray<FHitResult> GetActorsFromLineTrace(const FVector& _StartLocation, const FVector& _EndLocation);

	// Additional impact effects
	virtual void ApplyImpactEffects(UAbilitySystemComponent* _TargetASC) override;

	// CollisionChannel the weapon should detect
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Collision")
	TEnumAsByte<ECollisionChannel> CollisionChannelImpact;

//////////////// Animations
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UParticleSystem* LaserFX;

	virtual void FireAnimation() override;
};