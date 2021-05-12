#include "GA_FireWeaponProjectile.h"
#include "../../../Soldiers/Soldier.h"
#include "../AbilitySystemSoldier.h"
#include "SquadLeader/Soldiers/Soldier.h"

UGA_FireWeaponProjectile::UGA_FireWeaponProjectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::BasicAttack;
	AbilityID = ESoldierAbilityInputID::BasicAttack;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon.Projectile.Bazooka")));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Firing")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.ReloadingWeapon")));
}

void UGA_FireWeaponProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	SourceWeapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// TODO: Make this GA like fireWeaponInstant - Ammo are better handled and also the time between shoots
	SourceWeapon->DecrementAmmo();

	// Camera Shake
	SourceSoldier->ShakeCamera(SourceSoldier->GetCameraShakeFireClass());

	if (!CurrentActorInfo->IsNetAuthority())
		return;

	const FVector MuzzlePosition = SourceWeapon->GetWeaponMesh()->GetSocketLocation(SourceWeapon->GetMuzzleAttachPoint());

	FTransform Transform{ SourceSoldier->CurrentCameraComponent->GetForwardVector().Rotation(), MuzzlePosition};

	ASL_Projectile* Projectile = GetWorld()->SpawnActorDeferred<ASL_Projectile>(ProjectileClass, Transform, SourceSoldier, SourceSoldier, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Projectile)
	{
		if (SourceSoldier && SourceSoldier->GetTeam() && SourceSoldier->GetTeam()->Id == 1)
			Projectile->SetCollisionProfile(PN_Projectile1);
		else
			Projectile->SetCollisionProfile(PN_Projectile2);

		Projectile->FinishSpawning(Transform);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

bool UGA_FireWeaponProjectile::CanActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayTagContainer* _SourceTags, const FGameplayTagContainer* _TargetTags, OUT FGameplayTagContainer* _OptionalRelevantTags) const
{
	ASoldier* Soldier = Cast<ASoldier>(_ActorInfo->AvatarActor);
	return Soldier && Cast<ASL_Weapon>(Soldier->GetCurrentWeapon()) && Super::CanActivateAbility(_Handle, _ActorInfo, _SourceTags, _TargetTags, _OptionalRelevantTags);
}

void UGA_FireWeaponProjectile::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	SourceSoldier->OnStopFiring();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}