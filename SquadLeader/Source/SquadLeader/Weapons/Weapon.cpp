#include "Weapon.h"
#include "../Characters/Soldiers/Soldier.h"

AWeapon::AWeapon() : isNextFireReady{ true }, timeToReloadNextShoot{ 0.2f }, isAutomatic{ true }, penetration{ 10 }
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::tryFiring()
{
	ASoldier* soldier = Cast<ASoldier>(GetOwner());
	if (soldier && soldier->GetWantsToFire() && isNextFireReady)
		fire();
}

void AWeapon::fire()
{
	isNextFireReady = false;
	GetWorldTimerManager().SetTimer(timerReloadNextShoot, this, &AWeapon::onReadyToShoot, timeToReloadNextShoot, false);
}

void AWeapon::onReadyToShoot()
{
	isNextFireReady = true;
	if (isAutomatic)
		tryFiring();
}