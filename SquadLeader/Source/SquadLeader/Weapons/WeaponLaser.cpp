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

void AWeaponLaser::Fire()
{
	Super::Fire();

	ASoldier* Soldier = Cast<ASoldier>(GetOwner());
	if (!Soldier && GetLocalRole() != ROLE_Authority)
		return;

	TArray<FHitResult> outHits = GetActorsFromLineTrace(Soldier->GetActorLocation(), Soldier->lookingAtPosition());

	int remainPenetration = Penetration;
	for (auto it = outHits.begin(); it != outHits.end() && remainPenetration > 0; ++it)
	{
		if (auto actor = (*it).GetActor(); actor != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *actor->GetName()));

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
	objectTypes.AddObjectTypesToQuery(ECC_Pawn);

	// Ignore itself and the owner
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);
	collisionParams.AddIgnoredActor(Cast<ASoldier>(GetOwner()));

	// TODO: Find out how to handle multi detection. The line trace tracing stops at the first pawn.
	// TODO: See if lineTracing could use the penetration
	GetWorld()->LineTraceMultiByObjectType(outHits, _StartLocation, _EndLocation, objectTypes, collisionParams);
	DrawDebugLine(GetWorld(), _StartLocation, _EndLocation, FColor::Red, false, 6.f);

	return outHits;
}

void AWeaponLaser::ApplyImpactDamage(UAbilitySystemComponent* _TargetASC)
{
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), _TargetASC);
}

void AWeaponLaser::ApplyImpactEffects(UAbilitySystemComponent* _TargetASC)
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