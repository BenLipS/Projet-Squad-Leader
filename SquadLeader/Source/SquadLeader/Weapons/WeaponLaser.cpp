#include "WeaponLaser.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GamePlayStatics.h"
#include "../Soldiers/Soldier.h"
#include "SquadLeader/SquadLeader.h"
#include "Particles/ParticleSystemComponent.h"

AWeaponLaser::AWeaponLaser() : AWeapon(), CollisionChannelImpact{ ECC_Player }
{
}

void AWeaponLaser::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponLaser::Fire()
{
	ASoldier* Soldier = Cast<ASoldier>(GetOwner());
	if (!Soldier && GetLocalRole() != ROLE_Authority)
		return;

	Super::Fire();

	TArray<FHitResult> outHits = GetActorsFromLineTrace(Soldier->GetActorLocation(), Soldier->lookingAtPosition());

	int remainPenetration = Penetration;
	for (auto it = outHits.begin(); it != outHits.end() && remainPenetration > 0; ++it)
	{
		if (auto actor = (*it).GetActor(); actor != nullptr)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *actor->GetName()));

			if (auto soldierTarget = Cast<ASoldier>(actor); soldierTarget)
			{
				if (UAbilitySystemSoldier* soldierTargetASC = soldierTarget->GetAbilitySystemComponent(); soldierTargetASC)
				{
					ApplyImpactDamage(soldierTargetASC);
					ApplyImpactEffects(soldierTargetASC);
				}
			}
			--remainPenetration;
		}
	}
}

TArray<FHitResult> AWeaponLaser::GetActorsFromLineTrace(const FVector& _StartLocation, const FVector& _EndLocation)
{
	TArray<FHitResult> outHits;
	FCollisionObjectQueryParams objectTypes;
	objectTypes.AddObjectTypesToQuery(CollisionChannelImpact);

	// Ignore itself and the owner
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);
	collisionParams.AddIgnoredActor(Cast<ASoldier>(GetOwner()));

	GetWorld()->LineTraceMultiByObjectType(outHits, _StartLocation, _EndLocation, objectTypes, collisionParams);
	//DrawDebugLine(GetWorld(), _StartLocation, _EndLocation, FColor::Red, false, 6.f);

	return outHits;
}

void AWeaponLaser::ApplyImpactEffects(UAbilitySystemComponent* _TargetASC)
{
	// TODO: should be safe here since we call from Fire
	// Todo AvatarActor use getAvatarActor
	if (ASoldier* Soldier = Cast<ASoldier>(_TargetASC->AvatarActor); Soldier)
		Soldier->ShowImpactHitEffect();
}

void AWeaponLaser::FireAnimation()
{
	Super::FireAnimation();

	// TODO: should be safe here since we call from Fire
	if (ASoldier* Soldier = Cast<ASoldier>(GetOwner()); Soldier)
	{
		UParticleSystemComponent* LaserParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LaserFX, GetMuzzleLocation(), Soldier->GetSyncControlRotation() /*GetMuzzleRotation()*/);
	}
}