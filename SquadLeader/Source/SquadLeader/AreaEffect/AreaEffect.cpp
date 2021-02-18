// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaEffect.h"
#include "../Soldiers/Soldier.h"

// Sets default values

AAreaEffect::AAreaEffect() : realOwner{ this }, realOwnerHasASC{ true }
{
	// Set this actor to not call Tick() every frame.
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentAreaEffect>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAttributeSetAreaEffect>(TEXT("Attribute Set"));
}

AAreaEffect::AAreaEffect(AActor* realOwnerIn) : realOwner{ realOwnerIn }
{
	// Set this actor to not call Tick() every frame.
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentAreaEffect>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAttributeSetAreaEffect>(TEXT("Attribute Set"));

	if (Cast<IAbilitySystemInterface>(realOwner))
	{
		realOwnerHasASC = true;
	}
	else {
		realOwnerHasASC = false;
	}
}

UAbilitySystemComponentAreaEffect* AAreaEffect::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSetAreaEffect* AAreaEffect::GetAttributeSet() const
{
	return AttributeSet;
}

// Called when the game starts or when spawned
void AAreaEffect::BeginPlay()
{
	Super::BeginPlay();

	//AbilitySystemComponent->InitAbilityActorInfo(this, this);
	//AttributeSet = CreateDefaultSubobject<UAttributeSetAreaEffect>(TEXT("Attribute Set"));
	InitializeAttributes();

	OnAreaTick();

	if (AttributeSet->GetDuration() > 0.f) {
		if (!periodTimer.IsValid()) {
			GetWorldTimerManager().SetTimer(periodTimer, this, &AAreaEffect::OnAreaTick, AttributeSet->GetInterval(), true);
		}
		if (!areaTimer.IsValid()) {
			GetWorldTimerManager().SetTimer(areaTimer, this, &AAreaEffect::finishAreaEffect, AttributeSet->GetDuration(), false);
		}
	}
	else
	{
		finishAreaEffect();
	}
}

void AAreaEffect::finishAreaEffect()
{
	if (areaTimer.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(areaTimer);
	}
	if (periodTimer.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(periodTimer);
	}
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
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
	}
}

void AAreaEffect::OnAreaTick()
{
	//Test Actors in area
	FCollisionShape collisionShape;
	collisionShape.ShapeType = ECollisionShape::Sphere;
	collisionShape.SetSphere(AttributeSet->GetRadius());

	TArray<FHitResult> hitActors;

	FVector startTrace = GetActorLocation();
	FVector endTrace = startTrace;

	//Apply effect
	if (GetWorld()->SweepMultiByChannel(hitActors, startTrace, endTrace, FQuat::FQuat(), ECC_WorldStatic, collisionShape))
	{
		for (auto actor = hitActors.CreateIterator(); actor; ++actor)
		{
			if (actor->Actor == nullptr)
				continue;

			//UStaticMeshComponent* SM = Cast<UStaticMeshComponent>((*actor).Actor->GetRootComponent());
			ASoldier* soldier = Cast<ASoldier>((*actor).GetActor());

			if (soldier)
			{
				UAbilitySystemComponent* ASC;
				FGameplayEffectContextHandle EffectContext;
				if (realOwnerHasASC)
				{
					ASC = Cast<IAbilitySystemInterface>(realOwner)->GetAbilitySystemComponent();
					EffectContext = ASC->MakeEffectContext();
					EffectContext.AddSourceObject(realOwner);
				}
				else
				{
					ASC = soldier->GetAbilitySystemComponent();
					EffectContext = ASC->MakeEffectContext();
					EffectContext.AddSourceObject(soldier);
				}
				FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(ExplosionEffects, 1.f, EffectContext);
				if (NewHandle.IsValid())
					ASC->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), soldier->GetAbilitySystemComponent());
			}
		}
	}
}