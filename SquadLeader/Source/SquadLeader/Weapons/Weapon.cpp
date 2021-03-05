#include "Weapon.h"
#include "../Soldiers/Soldier.h"

AWeapon::AWeapon() : MaxAmmo{ 50 }, IsNextFireReady{ true }, TimeToReloadNextShoot{ 0.2f }, TimeToReloadAmmo{ 2.f }, IsAutomatic{ true }, Penetration{ 1 }, FieldOfViewAim{ 50.f }
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxAmmo;
}

void AWeapon::InitializeAbilitySystemComponent(UAbilitySystemSoldier* _abilitySystemComponent)
{
	AbilitySystemComponent = _abilitySystemComponent;
}

UAbilitySystemSoldier* AWeapon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

float AWeapon::GetFieldOfViewAim() const
{
	return FieldOfViewAim;
}

void AWeapon::ApplyImpactDamage(UAbilitySystemComponent* _targetASC)
{
}

void AWeapon::ApplyImpactEffects(UAbilitySystemComponent* _targetASC)
{
}

void AWeapon::TryFiring()
{
	ASoldier* soldier = Cast<ASoldier>(GetOwner());
	if (soldier && soldier->GetWantsToFire() && IsNextFireReady)
		Fire();
}

void AWeapon::TryFiring(const FGameplayEffectSpecHandle _damageEffectSpecHandle)
{
	DamageEffectSpecHandle = _damageEffectSpecHandle;
	DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);
	TryFiring();
}

void AWeapon::Fire()
{
	--CurrentAmmo;
	IsNextFireReady = false;

	if (CurrentAmmo == 0)
		GetWorldTimerManager().SetTimer(TimerReloadAmmo, this, &AWeapon::OnReloaded, TimeToReloadAmmo, false);
	else
		GetWorldTimerManager().SetTimer(TimerReloadNextShoot, this, &AWeapon::OnReadyToShoot, TimeToReloadNextShoot, false);

}

void AWeapon::OnReadyToShoot()
{
	IsNextFireReady = true;
	if (IsAutomatic)
		TryFiring();
}

void AWeapon::OnReloaded()
{
	CurrentAmmo = MaxAmmo;
	IsNextFireReady = true;
	if (IsAutomatic)
		TryFiring();
}