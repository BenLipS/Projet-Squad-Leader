#pragma once

#include "Core.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
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

	void OnAreaTick();

	// Define the effects applied by explosion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> ExplosionEffects;

	void FinishAreaEffect();
	void ApplyEffects(UAbilitySystemComponent* _TargetASC);
	void ApplyForce(AActor* _Actor);

	FTimerHandle AreaTimer;
	FTimerHandle PeriodTimer;

//////////////// Stats
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Duration", Replicated)
	float Duration = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Radius", Replicated)
	float Radius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Interval", Replicated)
	float Interval = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Impulse", Replicated)
	float StrenghImpulse = 1000.f;

//////////////// Collision
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Collision")
	bool bDebugTrace = false;

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