#include "WeaponProjectile.h"

AWeaponProjectile::AWeaponProjectile() : AWeapon()
{
}

void AWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponProjectile::Fire()
{
	Super::Fire();
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Shoot projectile weapon"));
}