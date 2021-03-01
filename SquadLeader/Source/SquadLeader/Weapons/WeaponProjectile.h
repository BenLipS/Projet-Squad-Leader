#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponProjectile.generated.h"

UCLASS()
class SQUADLEADER_API AWeaponProjectile : public AWeapon
{
	GENERATED_BODY()

public:
	AWeaponProjectile();
	void Fire() override;

protected:
	virtual void BeginPlay() override;
};
