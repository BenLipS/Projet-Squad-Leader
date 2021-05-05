#pragma once

#include "Core.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "Curves/CurveFloat.h"
#include "AreaEffect.generated.h"

class ASoldier;

UCLASS()
class SQUADLEADER_API AAreaEffect : public AActor
{
	GENERATED_BODY()

public:	
	AAreaEffect();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	ASoldier* SourceSoldier = nullptr;
	FName ProfileAreaEffectCollisionName;

//////////////// Effects
protected:
	FTimerHandle LifetimeTimer;
	FTimerHandle IntervalTimer;

	void OnReadyToApplyEffects();

	// Make sure there is no wall blocking the effect
	bool ValidateEffectOnSoldier(const FHitResult& _HitSoldier, const TArray<FHitResult>& _HitActors);
	void DestroyAreaEffect();

	void ApplyGameplayEffects(UAbilitySystemComponent* _TargetASC);
	void ApplyDamages(UAbilitySystemComponent* _TargetASC, const float _DistActorArea);
	void ApplyImpulse(AActor* _Actor, const float _DistActorArea);

	// Define the effects to apply
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<class UGameplayEffect>> ExplosionEffects;

//////////////// Stats
protected:
	// We can specifically define an execution GE for the damages. It will be combinated with the properties BaseDamage and CurveDamage so we can easily interpolate the damage. This is meant to be used with Soldier ASC
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Damage")
	TSubclassOf<UGameplayEffect> GE_DamageClass;

	// Damage to apply on the area effect. This only works with GE_DamageClass
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Damage")
	float DamageBase = 0.f;

	// Determine the current damage multiplier based on distance from the center. This only works with GE_DamageClass
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Damage")
	UCurveFloat* CurveDamage;

	float DetermineDamage(const float _DistActorArea) const;

	// Lifetime before destroying this area effect. Last at least one frame for the animation
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Duration", Replicated)
	float Lifetime = 0.f;

	// Radius of the area effect
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Radius", Replicated)
	float Radius = 100.f;

	// If we want to repeat the effects. Set the interval of time before redoing th effects
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Interval", Replicated)
	float Interval = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Impulse", Replicated)
	float ImpulseStrenghBase = 0.f;

	// Determine the current impulse strengh multiplier based on distance from the center
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Impulse", Replicated)
	UCurveFloat* CurveImpulseStrengh;

	FVector DetermineImpulse(AActor* _Actor, const float _DistActorArea) const;

//////////////// Collision
public:
	// Whether this area can apply its effect regardless the collisions
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Collision")
	bool bIgnoreBlock = true;

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Collision")
	bool bDebugTrace = false;

	void FilterTraceWithShield(TArray<FHitResult>& _HitResults);

//////////////// Animation
protected:
	void ShowAnimation();

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	UParticleSystem* AreaFX;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	FVector AreaFXRelativeLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	FRotator AreaFXRotator;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	FVector AreaFXScale;
};