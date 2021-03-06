#include "Weapon.h"
#include "../Soldiers/Soldier.h"

AWeapon::AWeapon() : MaxAmmo{ 50 }, IsNextFireReady{ true }, TimeToReloadAmmo{ 2.f }, TimeToReloadNextShoot{ 0.2f }, IsAutomatic{ true }, Penetration{ 1 }, FieldOfViewAim{ 50.f }
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxAmmo;
}

void AWeapon::InitializeAbilitySystemComponent(UAbilitySystemSoldier* _AbilitySystemComponent)
{
	AbilitySystemComponent = _AbilitySystemComponent;
}

UAbilitySystemSoldier* AWeapon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AWeapon::IsFullAmmo() const noexcept
{
	return CurrentAmmo == MaxAmmo;
}

float AWeapon::GetFieldOfViewAim() const noexcept
{
	return FieldOfViewAim;
}

void AWeapon::ApplyImpactDamage(UAbilitySystemComponent* _TargetASC)
{
}

void AWeapon::ApplyImpactEffects(UAbilitySystemComponent* _TargetASC)
{
}

void AWeapon::TryFiring()
{
	ASoldier* Soldier = Cast<ASoldier>(GetOwner());
	if (Soldier && Soldier->GetWantsToFire() && IsNextFireReady)
		Fire();
}

void AWeapon::TryFiring(const FGameplayEffectSpecHandle _DamageEffectSpecHandle)
{
	DamageEffectSpecHandle = _DamageEffectSpecHandle;
	DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);
	TryFiring();
}

void AWeapon::Fire()
{
	--CurrentAmmo;
	IsNextFireReady = false;

	if (CurrentAmmo == 0)
	{
		if (ASoldier* Soldier = Cast<ASoldier>(GetOwner()); Soldier)
			Soldier->ActivateAbility(ASoldier::SkillReloadWeaponTag);
		else
			Reload();
	}
	else
		GetWorldTimerManager().SetTimer(TimerReloadNextShoot, this, &AWeapon::OnReadyToShoot, TimeToReloadNextShoot, false);

}

void AWeapon::Reload()
{
	GetWorldTimerManager().SetTimer(TimerReloadAmmo, this, &AWeapon::OnReloaded, TimeToReloadAmmo, false);
}

void AWeapon::OnReadyToShoot()
{
	IsNextFireReady = true;
	if (IsAutomatic)
		TryFiring();
}

void AWeapon::OnReloaded()
{
	if (ASoldier* Soldier = Cast<ASoldier>(GetOwner()); Soldier)
		Soldier->CancelAbility(ASoldier::SkillReloadWeaponTag);

	CurrentAmmo = MaxAmmo;
	IsNextFireReady = true;
	if (IsAutomatic)
		TryFiring();
}