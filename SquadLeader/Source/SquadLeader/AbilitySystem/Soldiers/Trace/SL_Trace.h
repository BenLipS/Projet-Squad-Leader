// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WorldCollision.h"
#include "SL_Trace.generated.h"

/**
 * Reusable, configurable trace TargetActor. Subclass with your own trace shapes.
 * Meant to be used with GSAT_WaitTargetDataUsingActor instead of the default WaitTargetData AbilityTask as the default
 * one will destroy the TargetActor.
 */
UCLASS()
class SQUADLEADER_API ASL_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ASL_Trace();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Base targeting spread (degrees)
	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	float BaseSpread;

	// Aiming spread modifier
	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	float AimingSpreadMod;

	// Continuous targeting: spread increment
	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	float TargetingSpreadIncrement;

	// Continuous targeting: max increment
	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	float TargetingSpreadMax;

	// Current spread from continuous targeting
	UPROPERTY()
	float CurrentTargetingSpread;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	float MaxRange;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, config, meta = (ExposeOnSpawn = true), Category = "Trace")
	FCollisionProfileName TraceProfile;

	// Number of traces to perform at one time. Single bullet weapons like rilfes will only do one trace.
	// Multi-bullet weapons like shotguns can do multiple traces. Not intended to be used with PersistentHits.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	int32 NumberOfTraces;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bIgnoreBlockingHits;

	// HitResults will persist until Confirmation/Cancellation or until a new HitResult takes its place
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bUsePersistentHitResults;

	UFUNCTION(BlueprintCallable)
	virtual void ResetSpread();

	UFUNCTION()
	virtual float GetCurrentSpread() const;

	UFUNCTION(BlueprintCallable)
	void SetStartLocation(const FGameplayAbilityTargetingLocationInfo& InStartLocation);

	UFUNCTION(BlueprintCallable)
	FGameplayAbilityTargetingLocationInfo GetStartLocation() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetShouldProduceTargetDataOnServer(bool bInShouldProduceTargetDataOnServer);

	UFUNCTION(BlueprintCallable)
	void SetDestroyOnConfirmation(bool bInDestroyOnConfirmation = false);

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
	virtual void CancelTargeting() override;

	// Traces as normal, but will manually filter all hit actors
	virtual void LineTraceWithFilter(TArray<FHitResult>& OutHitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params);

	UFUNCTION()
	virtual bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition);

	UFUNCTION()
	virtual void StopTargeting();

	FVector GenerateRandomFireTrajectory(const FVector& _Start, FVector&& _End) const;

protected:
	// Trace End point, useful for debug drawing
	UPROPERTY()
	FVector CurrentTraceEnd;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AGameplayAbilityWorldReticle>> ReticleActors;

	UPROPERTY()
	TArray<FHitResult> PersistentHitResults;

	UFUNCTION()
	virtual FGameplayAbilityTargetDataHandle MakeTargetData(const TArray<FHitResult>& HitResults) const;

	UFUNCTION()
	virtual TArray<FHitResult> PerformTrace();

	virtual void DoTrace(TArray<FHitResult>& HitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params) PURE_VIRTUAL(ASL_Trace, return;);

	// Remove trace data in case a soldier shot on a shield from an ennemy team
	UFUNCTION(BlueprintCallable)
	void FilterTraceWithShield(TArray<FHitResult>& _HitResults);

	UFUNCTION()
	virtual void ShowDebugTrace(TArray<FHitResult>& HitResults, EDrawDebugTrace::Type DrawDebugType, float Duration = 2.0f) PURE_VIRTUAL(ASL_Trace, return;);
};
