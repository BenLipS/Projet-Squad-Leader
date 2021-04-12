// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "SL_Trace.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Engine/CollisionProfile.h"
#include "WorldCollision.h"
#include "SL_LineTrace.generated.h"

/**
 * Reusable, configurable line trace TargetActor.
 * Meant to be used with GSAT_WaitTargetDataUsingActor instead of the default WaitTargetData AbilityTask as the default
 * one will destroy the TargetActor.
 */
UCLASS()
class SQUADLEADER_API ASL_LineTrace : public ASL_Trace
{
	GENERATED_BODY()

public:
	ASL_LineTrace() = default;

protected:

	virtual void DoTrace(TArray<FHitResult>& HitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params) override;
	virtual void ShowDebugTrace(TArray<FHitResult>& HitResults, EDrawDebugTrace::Type DrawDebugType, float Duration = 2.0f) override;

#if ENABLE_DRAW_DEBUG
	// Util for drawing result of multi line trace from KismetTraceUtils.h
	void DrawDebugLineTraceMulti(const UWorld* World, const FVector& Start, const FVector& End, EDrawDebugTrace::Type DrawDebugType, bool bHit, const TArray<FHitResult>& OutHits, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime);
#endif // ENABLE_DRAW_DEBUG
};
