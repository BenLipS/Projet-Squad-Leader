#pragma once

#include "Core.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "../AbilitySystem/AreaEffect/AttributeSetAreaEffect.h"
#include "../AbilitySystem/AreaEffect/GameplayEffects/GE_Default_Stats_AreaEffect.h"
#include "../AbilitySystem/AreaEffect/AbilitySystemComponentAreaEffect.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AreaEffect.generated.h"

class ASoldier;

UCLASS()
class SQUADLEADER_API AAreaEffect : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:	
	AAreaEffect();

public:
	UAbilitySystemComponentAreaEffect* GetAbilitySystemComponent() const override;
	UAttributeSetAreaEffect* GetAttributeSet() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ASoldier* SourceSoldier = nullptr;

	//Finish the areaEffect
	void FinishAreaEffect();

	void InitializeAttributes();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System Component", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponentAreaEffect* AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Attribute Set", meta = (AllowPrivateAccess = "true"))
	UAttributeSetAreaEffect* AttributeSet;

	// Define the default stats
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attribute Set")
	TSubclassOf<class UGE_Default_Stats_AreaEffect> DefaultAttributeEffects;

	// Define the effect applied by explosion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> ExplosionEffects;

	void OnAreaTick();
	void ShowEffects();
	void ApplyEffects(UAbilitySystemComponent* _TargetASC);

	FTimerHandle AreaTimer;
	FTimerHandle PeriodTimer;

//////////////// Collision
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Collision")
	bool bDebugTrace = false;

//////////////// Particles
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	UParticleSystem* AreaFX;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	FVector AreaFXRelativeLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	FRotator AreaFXRotator;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Particles")
	FVector AreaFXScale;
};