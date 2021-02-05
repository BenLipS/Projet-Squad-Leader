#include "WeaponProjectile.h"

AWeaponProjectile::AWeaponProjectile() : AWeapon()
{
}

void AWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponProjectile::fire()
{
	Super::fire();
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Shoot projectile weapon"));
}