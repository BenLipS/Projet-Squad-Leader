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
};
