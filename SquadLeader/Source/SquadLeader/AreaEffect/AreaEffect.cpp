#include "AreaEffect.h"
#include "../Soldiers/Soldier.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

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
		for (auto Actor = HitActors.CreateIterator(); Actor; ++Actor)
		{
			if (Actor->Actor == nullptr)
				continue;

			if (ASoldier* TargetSoldier = Cast<ASoldier>((*Actor).GetActor()); TargetSoldier && TargetSoldier->GetAbilitySystemComponent())
				ApplyEffects(TargetSoldier->GetAbilitySystemComponent());
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

void AAreaEffect::ShowAnimation()
{
	UParticleSystemComponent* LevelUpParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AreaFX, FTransform{ FQuat{AreaFXRotator}, GetActorLocation() + AreaFXRelativeLocation, AreaFXScale });

#ifdef UE_BUILD_DEBUG
	if (bDebugTrace)
		DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 50, FColor::Blue, false, Duration);
#endif
}