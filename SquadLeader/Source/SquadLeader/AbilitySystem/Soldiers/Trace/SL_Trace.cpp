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
MaxHitResultsPerTrace{ 1 },
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

	// This is a lazy way of emptying and repopulating the ReticleActors.
	// We could come up with a solution that reuses them.
	DestroyReticleActors();

	if (ReticleClass)
	{
		for (int32 i = 0; i < MaxHitResultsPerTrace * NumberOfTraces; i++)
			SpawnReticleActor(GetActorLocation(), GetActorRotation());
	}

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

void ASL_Trace::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyReticleActors();

	Super::EndPlay(EndPlayReason);
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

	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);

	TArray<FHitResult> FilteredHitResults;

	// Start param could be player ViewPoint. We want HitResult to always display the StartLocation.
	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();

	for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
	{
		FHitResult& Hit = HitResults[HitIdx];

		if (!Hit.Actor.IsValid() || FilterHandle.FilterPassesForActor(Hit.Actor))
		{
			Hit.TraceStart = TraceStart;
			Hit.TraceEnd = End;

			FilteredHitResults.Add(Hit);
		}
	}
	OutHitResults = FilteredHitResults;
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

	DestroyReticleActors();

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

		// Remove extra hits to MaxHitResultsPerTrace
		for (int32 i = 0; i < TraceHitResults.Num() - MaxHitResultsPerTrace; ++i)
			TraceHitResults.Pop();

		for (int32 j = TraceHitResults.Num() - 1; j >= 0; --j)
		{
			FHitResult& HitResult = TraceHitResults[j];

			int32 ReticleIndex = TraceIndex * MaxHitResultsPerTrace + j;
			if (ReticleIndex < ReticleActors.Num())
			{
				if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActors[ReticleIndex].Get())
				{
					const bool bHitActor = HitResult.Actor != nullptr;

					if (bHitActor && !HitResult.bBlockingHit)
					{
						LocalReticleActor->SetActorHiddenInGame(false);

						const FVector ReticleLocation = (bHitActor && LocalReticleActor->bSnapToTargetedActor) ? HitResult.Actor->GetActorLocation() : HitResult.Location;

						LocalReticleActor->SetActorLocation(ReticleLocation);
						LocalReticleActor->SetIsTargetAnActor(bHitActor);
					}
					else
					{
						LocalReticleActor->SetActorHiddenInGame(true);
					}
				}
			}
		}

		if (TraceHitResults.Num() < 1)
		{
			// If there were no hits, add a default HitResult at the end of the trace
			FHitResult HitResult;
			// Start param could be player ViewPoint. We want HitResult to always display the StartLocation.
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

AGameplayAbilityWorldReticle* ASL_Trace::SpawnReticleActor(FVector Location, FRotator Rotation)
{
	if (ReticleClass)
	{
		AGameplayAbilityWorldReticle* SpawnedReticleActor = GetWorld()->SpawnActor<AGameplayAbilityWorldReticle>(ReticleClass, Location, Rotation);
		if (SpawnedReticleActor)
		{
			SpawnedReticleActor->InitializeReticle(this, MasterPC, ReticleParams);
			SpawnedReticleActor->SetActorHiddenInGame(true);
			ReticleActors.Add(SpawnedReticleActor);

			// This is to catch cases of playing on a listen server where we are using a replicated reticle actor.
			// (In a client controlled player, this would only run on the client and therefor never replicate. If it runs
			// on a listen server, the reticle actor may replicate. We want consistancy between client/listen server players.
			// Just saying 'make the reticle actor non replicated' isnt a good answer, since we want to mix and match reticle
			// actors and there may be other targeting types that want to replicate the same reticle actor class).
			if (!ShouldProduceTargetDataOnServer)
			{
				SpawnedReticleActor->SetReplicates(false);
			}

			return SpawnedReticleActor;
		}
	}

	return nullptr;
}

void ASL_Trace::DestroyReticleActors()
{
	for (int32 i = ReticleActors.Num() - 1; i >= 0; i--)
	{
		if (ReticleActors[i].IsValid())
		{
			ReticleActors[i].Get()->Destroy();
		}
	}

	ReticleActors.Empty();
}
