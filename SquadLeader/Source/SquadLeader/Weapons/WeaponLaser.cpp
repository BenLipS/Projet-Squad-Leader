#include "WeaponLaser.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GamePlayStatics.h"
#include "../Soldiers/Soldier.h"

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
	if (!soldierOwner && GetLocalRole() != ROLE_Authority)
		return;

	TArray<FHitResult> outHits = GetActorsFromLineTrace(soldierOwner->GetActorLocation(), soldierOwner->lookingAtPosition());

	int remainPenetration = penetration;
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

TArray<FHitResult> AWeaponLaser::GetActorsFromLineTrace(const FVector& _startLocation, const FVector& _endLocation)
{
	TArray<FHitResult> outHits;
	FCollisionObjectQueryParams objectTypes;
	objectTypes.AddObjectTypesToQuery(ECC_Pawn);

	// Ignore itself and the owner
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);
	collisionParams.AddIgnoredActor(Cast<ASoldier>(GetOwner()));

	// TODO: Find out how to handle multi detection. The line trace tracing stops at the first pawn.
	// TODO: See if lineTracing could use the penetration
	GetWorld()->LineTraceMultiByObjectType(outHits, _startLocation, _endLocation, objectTypes, collisionParams);
	DrawDebugLine(GetWorld(), _startLocation, _endLocation, FColor::Red, false, 6.f);

	return outHits;
}

void AWeaponLaser::ApplyImpactDamage(UAbilitySystemComponent* _targetASC)
{
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), _targetASC);
}

void AWeaponLaser::ApplyImpactEffects(UAbilitySystemComponent* _targetASC)
{
	if (ASoldier* soldierOwner = Cast<ASoldier>(GetOwner()); soldierOwner)
	{
		FGameplayEffectContextHandle EffectContext = _targetASC->MakeEffectContext();
		EffectContext.AddSourceObject(soldierOwner);

		for (TSubclassOf<UGameplayEffect> ImpactEffect : ImpactEffects)
		{
			FGameplayEffectSpecHandle NewHandle = _targetASC->MakeOutgoingSpec(ImpactEffect, soldierOwner->GetCharacterLevel(), EffectContext);
			if (NewHandle.IsValid())
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), _targetASC);
		}
	}
}