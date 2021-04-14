#include "AreaEffect.h"
#include "../Soldiers/Soldier.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AAreaEffect::AAreaEffect()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentAreaEffect>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAttributeSetAreaEffect>(TEXT("Attribute Set"));
}

UAbilitySystemComponentAreaEffect* AAreaEffect::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSetAreaEffect* AAreaEffect::GetAttributeSet() const
{
	return AttributeSet;
}

void AAreaEffect::BeginPlay()
{
	Super::BeginPlay();

	SourceSoldier = Cast<ASoldier>(GetInstigator());

	InitializeAttributes();
	OnAreaTick();

	if (AttributeSet->GetDuration() > 0.f)
	{
		if (!PeriodTimer.IsValid())
			GetWorldTimerManager().SetTimer(PeriodTimer, this, &AAreaEffect::OnAreaTick, AttributeSet->GetInterval(), true);

		if (!AreaTimer.IsValid())
			GetWorldTimerManager().SetTimer(AreaTimer, this, &AAreaEffect::FinishAreaEffect, AttributeSet->GetDuration(), false);
	}
	else
		FinishAreaEffect();
}

void AAreaEffect::FinishAreaEffect()
{
	if (AreaTimer.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(AreaTimer);

	if (PeriodTimer.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(PeriodTimer);

	Destroy();
}

void AAreaEffect::InitializeAttributes()
{
	if (!AbilitySystemComponent || !DefaultAttributeEffects)
		return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffects, 1.f, EffectContext);
	if (NewHandle.IsValid())
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
}

void AAreaEffect::OnAreaTick()
{
	ShowEffects();

	if (!SourceSoldier)
		return;

	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(AttributeSet->GetRadius());

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

void AAreaEffect::ShowEffects()
{
	UParticleSystemComponent* LevelUpParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AreaFX, FTransform{ FQuat{AreaFXRotator}, GetActorLocation() + AreaFXRelativeLocation, AreaFXScale });

#ifdef UE_BUILD_DEBUG
	if (bDebugTrace)
		DrawDebugSphere(GetWorld(), GetActorLocation(), AttributeSet->GetRadius(), 50, FColor::Blue, false, AttributeSet->GetDuration());
#endif
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