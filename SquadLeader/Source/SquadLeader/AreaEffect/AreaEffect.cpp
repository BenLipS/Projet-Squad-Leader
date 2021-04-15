#include "AreaEffect.h"
#include "../Soldiers/Soldier.h"
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

	DOREPLIFETIME(AAreaEffect, Duration);
	DOREPLIFETIME(AAreaEffect, Radius);
	DOREPLIFETIME(AAreaEffect, Interval);
	DOREPLIFETIME(AAreaEffect, CurveImpulseStrengh);
}

void AAreaEffect::BeginPlay()
{
	Super::BeginPlay();

	SourceSoldier = Cast<ASoldier>(GetInstigator());
	OnAreaTick();

	if (Duration > 0.f)
	{
		if (!PeriodTimer.IsValid())
			GetWorldTimerManager().SetTimer(PeriodTimer, this, &AAreaEffect::OnAreaTick, Interval, true);

		if (!AreaTimer.IsValid())
			GetWorldTimerManager().SetTimer(AreaTimer, this, &AAreaEffect::FinishAreaEffect, Duration, false);
	}
	else
		FinishAreaEffect();
}

void AAreaEffect::OnAreaTick()
{
	ShowAnimation();

	if (!SourceSoldier)
		return;

	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(Radius);

	TArray<FHitResult> HitActors;
	FVector StartTrace = GetActorLocation();
	FVector EndTrace = StartTrace;

	if (GetWorld()->SweepMultiByChannel(HitActors, StartTrace, EndTrace, FQuat::FQuat(), ECC_WorldStatic, CollisionShape))
	{
		for (auto TartgetActor = HitActors.CreateIterator(); TartgetActor; ++TartgetActor)
		{
			if (TartgetActor->Actor == nullptr)
				continue;

			if (ASoldier* TargetSoldier = Cast<ASoldier>(TartgetActor->GetActor()); TargetSoldier && TargetSoldier->GetAbilitySystemComponent())
			{
				UAbilitySystemComponent* ASC = TargetSoldier->GetAbilitySystemComponent();
				ApplyEffects(ASC);
				//ASC->AddLooseGameplayTag();
			}

			ApplyImpulse(TartgetActor->GetActor());
		}
	}
}

void AAreaEffect::FinishAreaEffect()
{
	if (AreaTimer.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(AreaTimer);

	if (PeriodTimer.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(PeriodTimer);

	Destroy();
}

void AAreaEffect::ApplyEffects(UAbilitySystemComponent* _TargetASC)
{
	if (UAbilitySystemComponent* ASC = SourceSoldier->GetAbilitySystemComponent(); ASC)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(SourceSoldier);

		for (TSubclassOf<UGameplayEffect> ExplosionEffect : ExplosionEffects)
		{
			FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(ExplosionEffect, SourceSoldier->GetCharacterLevel(), EffectContext);

			if (EffectSpecHandle.IsValid())
				ASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), _TargetASC);
		}
	}
}

void AAreaEffect::ApplyImpulse(AActor* _Actor)
{
	if (ACharacter* Character = Cast<ACharacter>(_Actor); Character)
		Character->GetCharacterMovement()->AddImpulse(DetermineImpulse(_Actor->GetActorLocation()));

	else if (UStaticMeshComponent * SM = Cast<UStaticMeshComponent>(_Actor->GetRootComponent()); SM && SM->Mobility == EComponentMobility::Movable)
		SM->AddImpulse(DetermineImpulse(_Actor->GetRootComponent()->GetComponentLocation()));
}

FVector AAreaEffect::DetermineImpulse(FVector _ActorLocation) const
{
	if (ImpulseStrenghBase <= 0.0f)
		return;

	const FVector DirectionFromAreaToActor = FVector{ _ActorLocation - GetActorLocation() }.GetSafeNormal();
	const FVector ImpulseDirection = DirectionFromAreaToActor.IsNormalized() ? DirectionFromAreaToActor : FVector{ 0.f, 0.f, 1.f };
	const float DistActorArea = FVector::Dist(_ActorLocation, GetActorLocation());

	FVector CurrentStrenghImpulse = ImpulseDirection * ImpulseStrenghBase;
	if (CurveImpulseStrengh)
		CurrentStrenghImpulse *= CurveImpulseStrengh->GetFloatValue(DistActorArea / Radius);

	return CurrentStrenghImpulse;
}

void AAreaEffect::ShowAnimation()
{
	UParticleSystemComponent* LevelUpParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AreaFX, FTransform{ FQuat{AreaFXRotator}, GetActorLocation() + AreaFXRelativeLocation, AreaFXScale });

#ifdef UE_BUILD_DEBUG
	if (bDebugTrace)
		DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 50, FColor::Blue, false, Duration);
#endif
}