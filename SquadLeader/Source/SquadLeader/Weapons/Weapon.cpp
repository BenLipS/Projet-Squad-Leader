#include "Weapon.h"
#include "../Soldiers/Soldier.h"
#include "../Soldiers/Players/SoldierPlayer.h"
#include "../Soldiers/Players/SoldierPlayerController.h"
#include "../UI/PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon() : MaxAmmo{ 50 }, IsNextFireReady{ true }, TimeToReloadAmmo{ 2.f }, TimeToReloadNextShoot{ 0.2f }, IsAutomatic{ true }, Penetration{ 1 }, FieldOfViewAim{ 50.f }, FireMuzzleFXScale{ FVector{1.f} }
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetMaxAmmo(MaxAmmo); //HUD Update
	SetAmmo(MaxAmmo);

	if (ASoldier* Soldier = Cast<ASoldier>(GetOwner()); Soldier && !Mesh)
		Mesh = Soldier->GetMesh();
}

void AWeapon::OnRep_Owner()
{
	if (ASoldier* Soldier = Cast<ASoldier>(GetOwner()); Soldier && !Mesh)
		Mesh = Soldier->GetMesh();
}

void AWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AWeapon::InitializeAbilitySystemComponent(UAbilitySystemSoldier* _AbilitySystemComponent)
{
	AbilitySystemComponent = _AbilitySystemComponent;
}

UAbilitySystemSoldier* AWeapon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AWeapon::SetAmmo(uint8 newAmmo)
{
	if (ASoldierPlayer* SP = GetOwner<ASoldierPlayer>(); SP)
	{
		if (ASoldierPlayerController* PC = SP->GetController<ASoldierPlayerController>(); PC)
		{
			if (APlayerHUD* HUD = PC->GetHUD<APlayerHUD>(); HUD)
			{
				HUD->OnAmmoChanged(newAmmo);
			}
		}
	}
	CurrentAmmo = newAmmo;
}

void AWeapon::SetMaxAmmo(uint8 newMaxAmmo)
{
	if (ASoldierPlayer* SP = GetOwner<ASoldierPlayer>(); SP)
	{
		if (ASoldierPlayerController* PC = SP->GetController<ASoldierPlayerController>(); PC)
		{
			if (APlayerHUD* HUD = PC->GetHUD<APlayerHUD>(); HUD)
			{
				HUD->OnMaxAmmoChanged(newMaxAmmo);
			}
		}
	}
	MaxAmmo = newMaxAmmo;
}

uint8 AWeapon::GetAmmo()
{
	return CurrentAmmo;
}

uint8 AWeapon::GetMaxAmmo()
{
	return MaxAmmo;
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
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), _TargetASC);
}

void AWeapon::ApplyImpactEffects(UAbilitySystemComponent* _TargetASC)
{
	if (ASoldier* Soldier = Cast<ASoldier>(GetOwner()); Soldier)
	{
		FGameplayEffectContextHandle EffectContext = _TargetASC->MakeEffectContext();
		EffectContext.AddSourceObject(Soldier);

		for (TSubclassOf<UGameplayEffect> ImpactEffect : ImpactEffects)
		{
			FGameplayEffectSpecHandle NewHandle = _TargetASC->MakeOutgoingSpec(ImpactEffect, Soldier->GetCharacterLevel(), EffectContext);
			if (NewHandle.IsValid())
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), _TargetASC);
		}
	}
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
	ASoldier* Soldier = Cast<ASoldier>(GetOwner());
	if (!Soldier)
		return;

	if (HasAuthority())
		MulticastFireAnimation();
	else
		ServerFireAnimation();

	SetAmmo(CurrentAmmo - 1);
	IsNextFireReady = false;

	if (CurrentAmmo == 0)
		Soldier->ActivateAbility(ASoldier::SkillReloadWeaponTag);
	else
		GetWorldTimerManager().SetTimer(TimerReloadNextShoot, this, &AWeapon::OnReadyToShoot, TimeToReloadNextShoot, false);

}

void AWeapon::FireAnimation()
{
	// TODO: should be safe here since we call from Fire
	if (ASoldier* Soldier = Cast<ASoldier>(GetOwner()); Soldier)
	{
		// TODO: Perhaps give a default value to avoid this if. It could be expensive for rapid fires
		if (UAnimMontage* FireMontage = Soldier->WeaponFireMontage; FireMontage)
			Soldier->PlayAnimMontage(FireMontage);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireMuzzleFX, GetMuzzleLocation(), Soldier->GetSyncControlRotation() /*GetMuzzleRotation()*/, FireMuzzleFXScale);
	}
}

void AWeapon::MulticastFireAnimation_Implementation()
{
	FireAnimation();
}

bool AWeapon::MulticastFireAnimation_Validate()
{
	return true;
}

void AWeapon::ServerFireAnimation_Implementation()
{
	FireAnimation();
}

bool AWeapon::ServerFireAnimation_Validate()
{
	return true;
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

	SetAmmo(MaxAmmo);
	IsNextFireReady = true;
	if (IsAutomatic)
		TryFiring();
}

FVector AWeapon::GetMuzzleLocation() const
{
	// TODO: Should have a mesh anytime
	return Mesh ? Mesh->GetSocketLocation(FireMuzzleAttachPoint) : FVector{0.f};
}
FRotator AWeapon::GetMuzzleRotation() const
{
	// TODO: Should have a mesh anytime
	return Mesh ? Mesh->GetSocketRotation(FireMuzzleAttachPoint) : FRotator{ 0.f };
}