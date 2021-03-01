#include "AreaEffect.h"
#include "DrawDebugHelpers.h"
#include "../Soldiers/Soldier.h"

AAreaEffect::AAreaEffect() : realOwner{ this }, realOwnerHasASC{ true }
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentAreaEffect>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAttributeSetAreaEffect>(TEXT("Attribute Set"));
}

AAreaEffect::AAreaEffect(AActor* realOwnerIn) : realOwner{ realOwnerIn }
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentAreaEffect>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAttributeSetAreaEffect>(TEXT("Attribute Set"));

	if (Cast<IAbilitySystemInterface>(realOwner))
	{
		realOwnerHasASC = true;
	}
	else
	{
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

void AAreaEffect::BeginPlay()
{
	Super::BeginPlay();

	InitializeAttributes();
	OnAreaTick();

	if (AttributeSet->GetDuration() > 0.f)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), AttributeSet->GetRadius(), 50, FColor::Blue, false, AttributeSet->GetDuration());
		if (!periodTimer.IsValid()) {
			GetWorldTimerManager().SetTimer(periodTimer, this, &AAreaEffect::OnAreaTick, AttributeSet->GetInterval(), true);
		}
		if (!areaTimer.IsValid()) {
			GetWorldTimerManager().SetTimer(areaTimer, this, &AAreaEffect::finishAreaEffect, AttributeSet->GetDuration(), false);
		}
	}
	else
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), AttributeSet->GetRadius(), 50, FColor::Red, false, 0.5f);
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

	//Apply effects
	if (GetWorld()->SweepMultiByChannel(hitActors, startTrace, endTrace, FQuat::FQuat(), ECC_WorldStatic, collisionShape))
	{
		for (auto actor = hitActors.CreateIterator(); actor; ++actor)
		{
			if (actor->Actor == nullptr)
				continue;

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
				for (auto effect : ExplosionEffects)
				{
					auto realEffect = Cast<UGameplayEffect>(effect);
					realEffect;
					FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(effect, 1.f, EffectContext);

					if (NewHandle.IsValid())
					{
						auto inter = NewHandle.Data.Get();
						ASC->ApplyGameplayEffectSpecToTarget(*inter, soldier->GetAbilitySystemComponent());
					}
				}
			}
		}
	}
}