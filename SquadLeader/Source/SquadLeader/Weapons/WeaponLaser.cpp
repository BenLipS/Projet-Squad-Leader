#include "WeaponLaser.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GamePlayStatics.h"
#include "../Characters/Soldiers/Soldier.h"

AWeaponLaser::AWeaponLaser() : AWeapon()
{
}

void AWeaponLaser::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponLaser::fire()
{
	Super::fire();

	ASoldier* soldierOwner = Cast<ASoldier>(GetOwner());
	if (!soldierOwner)
		return;

	FVector startLocation = soldierOwner->GetActorLocation();//GetActorLocation();
	FVector endLocation = soldierOwner->lookingAtPosition();
	auto endTrace = endLocation - startLocation;
	endTrace.Normalize();
	DrawDebugLine(GetWorld(), startLocation, startLocation + (endTrace * 10000), FColor::Red, false, 6.f);

	TArray<FHitResult> outHits;
	FCollisionObjectQueryParams objectTypes;
	objectTypes.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);
	collisionParams.AddIgnoredActor(soldierOwner);

	// TODO: Find out how to handle multi detection. The line trace tracing stops at the first pawn.
	GetWorld()->LineTraceMultiByObjectType(outHits, startLocation, endLocation, objectTypes, collisionParams);

	int objectPenetrated = penetration;
	for (auto it = outHits.begin(); it != outHits.end() && objectPenetrated > 0; ++it)
	{
		if (auto actor = (*it).GetActor(); actor != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *actor->GetName()));

			if (auto soldier = Cast<ASoldier>(actor); soldier)
				--objectPenetrated;
		}
	}
}