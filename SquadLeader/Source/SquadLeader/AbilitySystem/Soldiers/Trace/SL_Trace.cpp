// Copyright 2020 Dan Kestranek.

#include "SL_Trace.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "GameplayAbilitySpec.h"
#include "SquadLeader/Soldiers/Soldier.h"

ASL_Trace::ASL_Trace():
BaseSpread{ 0.f },
AimingSpreadMod{ 1.f },
TargetingSpreadIncrement{ 0.f },
TargetingSpreadMax{ 0.f },
CurrentTargetingSpread{ 0.f },
MaxRange{ 999999.f },
NumberOfTraces{ 1 },
bIgnoreBlockingHits{ false },
bUsePersistentHitResults{ false }
{
	bDestroyOnConfirmation = false;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
}

void ASL_Trace::ResetSpread()
{
	BaseSpread = 0.0f;
	AimingSpreadMod = 1.0f;
	TargetingSpreadIncrement = 0.0f;
	TargetingSpreadMax = 0.0f;
	CurrentTargetingSpread = 0.0f;
}

float ASL_Trace::GetCurrentSpread() const
{
	float FinalSpread = BaseSpread + CurrentTargetingSpread;

	UAbilitySystemComponent* ASC = OwningAbility->GetCurrentActorInfo()->AbilitySystemComponent.Get();
	
	if (ASC && ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Aiming"))))
		return FinalSpread * AimingSpreadMod;
	return FinalSpread;
}

void ASL_Trace::SetStartLocation(const FGameplayAbilityTargetingLocationInfo& InStartLocation)
{
	StartLocation = InStartLocation;
}

void ASL_Trace::SetShouldProduceTargetDataOnServer(bool bInShouldProduceTargetDataOnServer)
{
	ShouldProduceTargetDataOnServer = bInShouldProduceTargetDataOnServer;
}

void ASL_Trace::SetDestroyOnConfirmation(bool bInDestroyOnConfirmation)
{
	bDestroyOnConfirmation = bDestroyOnConfirmation;
}

void ASL_Trace::StartTargeting(UGameplayAbility* Ability)
{
	// Don't call to Super because we can have more than one Reticle

	SetActorTickEnabled(true);

	OwningAbility = Ability;
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();

	if (bUsePersistentHitResults)
		PersistentHitResults.Empty();
}

void ASL_Trace::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		TArray<FHitResult> HitResults = PerformTrace();
		FGameplayAbilityTargetDataHandle Handle = MakeTargetData(HitResults);
		TargetDataReadyDelegate.Broadcast(Handle);

#if ENABLE_DRAW_DEBUG
		if (bDebug)
			ShowDebugTrace(HitResults, EDrawDebugTrace::Type::ForDuration, 2.0f);
#endif
	}

	if (bUsePersistentHitResults)
		PersistentHitResults.Empty();
}

void ASL_Trace::CancelTargeting()
{
	const FGameplayAbilityActorInfo* ActorInfo = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
	UAbilitySystemComponent* ASC = (ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);
	if (ASC)
	{
		ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericCancel, OwningAbility->GetCurrentAbilitySpecHandle(), OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey()).Remove(GenericCancelHandle);
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("AGameplayAbilityTargetActor::CancelTargeting called with null ASC! Actor %s"), *GetName());
	}

	CanceledDelegate.Broadcast(FGameplayAbilityTargetDataHandle());

	SetActorTickEnabled(false);

	if (bUsePersistentHitResults)
		PersistentHitResults.Empty();
}

void ASL_Trace::BeginPlay()
{
	Super::BeginPlay();

	// Start with Tick disabled. We'll enable it in StartTargeting() and disable it again in StopTargeting().
	// For instant confirmations, tick will never happen because we StartTargeting(), ConfirmTargeting(), and immediately StopTargeting().
	SetActorTickEnabled(false);
}

void ASL_Trace::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TArray<FHitResult> HitResults;
	if (bDebug || bUsePersistentHitResults)
	{
		// Only need to trace on Tick if we're showing debug or if we use persistent hit results, otherwise we just use the confirmation trace
		HitResults = PerformTrace();
	}

#if ENABLE_DRAW_DEBUG
	if (SourceActor && bDebug)
		ShowDebugTrace(HitResults, EDrawDebugTrace::Type::ForOneFrame);
#endif
}

void ASL_Trace::LineTraceWithFilter(TArray<FHitResult>& OutHitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
{
	check(World);

	FHitResult HitResult;
	World->LineTraceSingleByProfile(HitResult, Start, End, ProfileName, Params);

	OutHitResults = { HitResult };
	return;
}

bool ASL_Trace::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition)
{
	FVector CameraToCenter = AbilityCenter - CameraLocation;
	float DotToCenter = FVector::DotProduct(CameraToCenter, CameraDirection);
	if (DotToCenter >= 0)		//If this fails, we're pointed away from the center, but we might be inside the sphere and able to find a good exit point.
	{
		float DistanceSquared = CameraToCenter.SizeSquared() - (DotToCenter * DotToCenter);
		float RadiusSquared = (AbilityRange * AbilityRange);
		if (DistanceSquared <= RadiusSquared)
		{
			float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
			float DistanceAlongRay = DotToCenter + DistanceFromCamera;						//Subtracting instead of adding will get the other intersection point
			ClippedPosition = CameraLocation + (DistanceAlongRay * CameraDirection);		//Cam aim point clipped to range sphere
			return true;
		}
	}
	return false;
}

void ASL_Trace::StopTargeting()
{
	SetActorTickEnabled(false);

	// Clear added callbacks
	TargetDataReadyDelegate.Clear();
	CanceledDelegate.Clear();

	if (GenericDelegateBoundASC)
	{
		GenericDelegateBoundASC->GenericLocalConfirmCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::ConfirmTargeting);
		GenericDelegateBoundASC->GenericLocalCancelCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::CancelTargeting);
		GenericDelegateBoundASC = nullptr;
	}
}

FVector ASL_Trace::GenerateRandomFireTrajectory(const FVector& _Start, FVector&& _End) const
{
	FVector TrajectoryNormal = (_End - _Start).GetSafeNormal();
	if (TrajectoryNormal.IsZero())
		TrajectoryNormal = {1.f, 0.f, 0.f}; // TODO What should be the default ?

	const float ConeHalfAngle = FMath::DegreesToRadians(GetCurrentSpread() * 0.5f);
	FRandomStream WeaponRandomStream(FMath::Rand());
	const FVector ShootDir = WeaponRandomStream.VRandCone(TrajectoryNormal, ConeHalfAngle, ConeHalfAngle);

	return _Start + ShootDir * MaxRange;
}

FGameplayAbilityTargetDataHandle ASL_Trace::MakeTargetData(const TArray<FHitResult>& HitResults) const
{
	FGameplayAbilityTargetDataHandle ReturnDataHandle;

	for (int32 i = 0; i < HitResults.Num(); i++)
	{
		/** Note: These are cleaned up by the FGameplayAbilityTargetDataHandle (via an internal TSharedPtr) */
		FGameplayAbilityTargetData_SingleTargetHit* ReturnData = new FGameplayAbilityTargetData_SingleTargetHit();
		ReturnData->HitResult = HitResults[i];
		ReturnDataHandle.Add(ReturnData);
	}

	return ReturnDataHandle;
}

TArray<FHitResult> ASL_Trace::PerformTrace()
{
	ASoldier* Soldier = Cast<ASoldier>(SourceActor);
	ensure(Soldier);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd = TraceEnd = Soldier->GetLookingAtPosition();

	// ClipCameraRayToAbilityRange(ViewStart, ViewDir, TraceStart, MaxRange, ViewEnd);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AGSGATA_LineTrace), false);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(SourceActor);
	Params.bIgnoreBlocks = false;

	TArray<FHitResult> ReturnHitResults;

	for (int32 TraceIndex = 0; TraceIndex < NumberOfTraces; ++TraceIndex)
	{
		CurrentTargetingSpread = FMath::Min(TargetingSpreadMax, CurrentTargetingSpread + TargetingSpreadIncrement);

		TraceEnd = GenerateRandomFireTrajectory(TraceStart, MoveTemp(TraceEnd));
		CurrentTraceEnd = TraceEnd;

		SetActorLocationAndRotation(TraceEnd, SourceActor->GetActorRotation());

		TArray<FHitResult> TraceHitResults;
		DoTrace(TraceHitResults, SourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);

		if (TraceHitResults.Num() < 1)
		{
			// If there were no hits, add a default HitResult at the end of the trace
			FHitResult HitResult;
			HitResult.TraceStart = StartLocation.GetTargetingTransform().GetLocation();
			HitResult.TraceEnd = TraceEnd;
			HitResult.Location = TraceEnd;
			HitResult.ImpactPoint = TraceEnd;
			TraceHitResults.Add(HitResult);
		}
		ReturnHitResults.Append(TraceHitResults);
	}
	return ReturnHitResults;
}