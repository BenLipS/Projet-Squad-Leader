#include "AreaEffect.h"
#include "../Soldiers/Soldier.h"
#include "../Soldiers/Players/SoldierPlayerController.h"
#include "SquadLeader/Weapons/Shield.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"

#ifdef UE_BUILD_DEBUG
#include "DrawDebugHelpers.h"
#endif

AAreaEffect::AAreaEffect()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void AAreaEffect::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAreaEffect, Lifetime);
	DOREPLIFETIME(AAreaEffect, Radius);
	DOREPLIFETIME(AAreaEffect, Interval);
	DOREPLIFETIME(AAreaEffect, ImpulseStrenghBase);
	DOREPLIFETIME(AAreaEffect, CurveImpulseStrengh);
}

void AAreaEffect::BeginPlay()
{
	Super::BeginPlay();
	SourceSoldier = Cast<ASoldier>(GetInstigator());

	ProfileAreaEffectCollisionName = FName{ PN_AreaEffect2 };
	if (SourceSoldier && SourceSoldier->GetTeam() && SourceSoldier->GetTeam()->Id == 1)
		ProfileAreaEffectCollisionName = FName{ PN_AreaEffect1 };

	// Apply effect at least once
	OnReadyToApplyEffects();

	if (Lifetime > 0.f)
	{
		GetWorldTimerManager().SetTimer(IntervalTimer, this, &AAreaEffect::OnReadyToApplyEffects, Interval, true);
		GetWorldTimerManager().SetTimer(LifetimeTimer, this, &AAreaEffect::DestroyAreaEffect, Lifetime, false);
	}
	else
		DestroyAreaEffect();
}

void AAreaEffect::OnReadyToApplyEffects()
{
	ShowAnimation();

	if (!SourceSoldier)
		return;

	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(Radius);

	TArray<FHitResult> HitActors;
	const FVector StartTrace = GetActorLocation();
	const FVector EndTrace = StartTrace;

	FCollisionQueryParams QueryParams{};
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->SweepMultiByProfile(HitActors, StartTrace, EndTrace, FQuat::FQuat(), ProfileAreaEffectCollisionName, CollisionShape, QueryParams))
	{
		for (int32 i = 0; i < HitActors.Num(); ++i)
		{
			AActor* TargetActor = HitActors[i].GetActor();
			if (TargetActor == nullptr)
				continue;

			const float DistActorArea = FVector::Dist(TargetActor->GetActorLocation(), GetActorLocation());

			if (ASoldier* TargetSoldier = Cast<ASoldier>(TargetActor); TargetSoldier && TargetSoldier->GetAbilitySystemComponent())
			{
				// Make sure there is no object blocking the effect
				TArray<FHitResult> HitActorsBeforeSoldier = HitActors;
				HitActorsBeforeSoldier.RemoveAt(i, HitActors.Num() - i);

				if (!ValidateEffectOnSoldier(HitActors[i], HitActorsBeforeSoldier))
					continue;

				UAbilitySystemComponent* TargetASC = TargetSoldier->GetAbilitySystemComponent();
				ApplyDamages(TargetASC, DistActorArea);
				ApplyGameplayEffects(TargetASC);
			}

			ApplyImpulse(TargetActor, DistActorArea);
		}
	}
}

bool AAreaEffect::ValidateEffectOnSoldier(const FHitResult& _HitSoldier, const TArray<FHitResult>& _HitActors)
{
	if (bIgnoreBlock || _HitActors.Num() <= 0) // No object between the soldier and the area effect
		return true;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	ECollisionChannel CollisionChannel = ECC_Projectile2;
	if (SourceSoldier && SourceSoldier->GetTeam() && SourceSoldier->GetTeam()->Id == 1)
		CollisionChannel = ECC_Projectile1;

	const FVector StartTrace = GetActorLocation();
	const FVector EndTrace = _HitSoldier.ImpactPoint;
	GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace + 10.f * (EndTrace - StartTrace), CollisionChannel, QueryParams);

	FilterTraceWithShield(HitResults);

	return HitResults.Num() > 0 && HitResults.Last().Actor == _HitSoldier.Actor;
}

void AAreaEffect::DestroyAreaEffect()
{
	if (LifetimeTimer.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(LifetimeTimer);

	if (IntervalTimer.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(IntervalTimer);

	Destroy();
}

void AAreaEffect::ApplyGameplayEffects(UAbilitySystemComponent* _TargetASC)
{
	if (UAbilitySystemComponent* SourceASC = SourceSoldier->GetAbilitySystemComponent(); SourceASC)
	{
		FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
		EffectContext.AddSourceObject(SourceSoldier);

		for (TSubclassOf<UGameplayEffect> ExplosionEffect : ExplosionEffects)
		{
			FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(ExplosionEffect, SourceSoldier->GetCharacterLevel(), EffectContext);

			if (EffectSpecHandle.IsValid())
				SourceASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), _TargetASC);
		}
	}
}

void AAreaEffect::ApplyDamages(UAbilitySystemComponent* _TargetASC, const float _DistActorArea)
{
	if (!GE_DamageClass)
		return;

	if (UAbilitySystemComponent* SourceASC = SourceSoldier->GetAbilitySystemComponent(); SourceASC)
	{
		FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
		FGameplayEffectSpecHandle DamageEffectSpecHandle = SourceASC->MakeOutgoingSpec(GE_DamageClass, SourceSoldier->GetCharacterLevel(), EffectContext);

		const float Damage = DetermineDamage(_DistActorArea);
		DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);
		SourceASC->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), _TargetASC);

		// Notify HUD for hit marker
		if (ASoldierPlayerController* PC = SourceSoldier->GetController<ASoldierPlayerController>(); PC)
			PC->NotifySoldierHit(Damage, false);
	}
}

void AAreaEffect::ApplyImpulse(AActor* _Actor, const float _DistActorArea)
{
	if (ASoldier* Soldier = Cast<ASoldier>(_Actor); Soldier)
	{
		Soldier->GetCharacterMovement()->AddImpulse(DetermineImpulse(_Actor, _DistActorArea));
		//
		// TODO uncomment camerashake - Each area effect shall have one
		//
		//
		//
		//Soldier->ShakeCamera();
	}

	//else if (UStaticMeshComponent * SM = Cast<UStaticMeshComponent>(_Actor->GetRootComponent()); SM && SM->Mobility == EComponentMobility::Movable)
	//	SM->AddImpulse(DetermineImpulse(_Actor, _DistActorArea));
}

float AAreaEffect::DetermineDamage(const float _DistActorArea) const
{
	return CurveDamage ? FMath::RoundToFloat(DamageBase * CurveDamage->GetFloatValue(_DistActorArea / Radius)) : DamageBase;
}

FVector AAreaEffect::DetermineImpulse(AActor* _Actor, const float _DistActorArea) const
{
	if (ImpulseStrenghBase <= 0.0f)
		return FVector{0.f};

	const FVector DirectionFromAreaToActor = FVector{ _Actor->GetActorLocation() - GetActorLocation() }.GetSafeNormal();
	const FVector ImpulseDirection = DirectionFromAreaToActor.IsNormalized() ? DirectionFromAreaToActor : FVector{ 0.f, 0.f, 1.f };

	FVector CurrentStrenghImpulse = ImpulseDirection * ImpulseStrenghBase;
	if (CurveImpulseStrengh)
		CurrentStrenghImpulse *= CurveImpulseStrengh->GetFloatValue(_DistActorArea / Radius);

	return CurrentStrenghImpulse;
}

void AAreaEffect::FilterTraceWithShield(TArray<FHitResult>& _HitResults)
{
	ASoldier* SoldierShooter = Cast<ASoldier>(GetInstigator());

	for (int32 i = 0; i < _HitResults.Num(); ++i)
	{
		if (AShield* Shield = Cast<AShield>(_HitResults[i].Actor); Shield)
		{
			_HitResults.RemoveAt(FMath::Min(i + 1, _HitResults.Num() - 1), _HitResults.Num() - i - 1);
			return;
		}
	}
}

void AAreaEffect::ShowAnimation()
{
	UParticleSystemComponent* LevelUpParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AreaFX, FTransform{ FQuat{AreaFXRotator}, GetActorLocation() + AreaFXRelativeLocation, AreaFXScale });

#ifdef UE_BUILD_DEBUG
	if (bDebugTrace)
		DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 50, FColor::Blue, false, Lifetime);
#endif
}