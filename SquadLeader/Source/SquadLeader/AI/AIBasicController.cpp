// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBasicController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Math/Vector.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "DrawDebugHelpers.h"


AAIBasicController::AAIBasicController() :
	AlignementWeight{ 100.f },
	CohesionWeight{ 100.f },
	SeparationWeight{ 200.f },
	ObjectifWeight{ 500.f },
	MovementVectorScale{ 2.f },
	SeparationVector{ 0.f, 0.f, 0.f },
	CohesionVector{ 0.f, 0.f, 0.f },
	AlignementVector{ 0.f, 0.f, 0.f },
	ObjectifLocation{ 1000.f, 0.f, 10.f },
	MovementVector{ 0.f, 0.f, 0.f },
	ObjectifVector{ 0.f, 0.f, 0.f }
{
	setup_BehaviorTree();
}


void AAIBasicController::BeginPlay() {
	Super::BeginPlay();
}

void AAIBasicController::ResetVectors()
{
	SeenBoids.Empty();
	AlignementVector = FVector::ZeroVector;
	CohesionVector = FVector::ZeroVector;
	SeparationVector = FVector::ZeroVector;
	ObjectifVector = FVector::ZeroVector;
}

void AAIBasicController::UpdateNeighbourhood()
{
	for (ASoldier* soldier : SeenSoldier)
	{
		if (AAIBasicController* AIBasic = Cast<AAIBasicController>(soldier->Controller); AIBasic)
			SeenBoids.Add(AIBasic);
	};
}

void AAIBasicController::UpdateCohesionVector()
{
	FVector SoldierLocation = GetPawn()->GetActorLocation();
	for (AAIBasicController* Boid : SeenBoids)
	{
		CohesionVector += Boid->GetPawn()->GetActorLocation() - SoldierLocation;
	}

	CohesionVector = (CohesionVector / SeenBoids.Num()) / 100;

}

void AAIBasicController::UpdateAlignementVector()
{
	for (AAIBasicController* Boid : SeenBoids)
	{
		AlignementVector += Boid->MovementVector.GetSafeNormal(DefaultNormalizeVectorTolerance);
	}

	AlignementVector = (MovementVector + AlignementVector).GetSafeNormal(DefaultNormalizeVectorTolerance);
}

void AAIBasicController::UpdateSeparationVector()
{
	FVector SoldierLocation = GetPawn()->GetActorLocation();

	for (AAIBasicController* Boid : SeenBoids)
	{
		FVector Separation = SoldierLocation - Boid->GetPawn()->GetActorLocation();
		SeparationVector += Separation.GetSafeNormal(DefaultNormalizeVectorTolerance) / FMath::Abs(Separation.Size() - BoidPhysicalRadius);
	}

	const FVector SeparationForceComponent = SeparationVector * 100;
	SeparationVector += SeparationForceComponent + SeparationForceComponent * (5 / SeenBoids.Num());
}

void AAIBasicController::UpdateObjectifVector()
{
	FVector SoldierLocation = GetPawn()->GetActorLocation();
	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), SoldierLocation, ObjectifLocation, NULL);
	
	FVector ObjectifLocalDir;
	if (path->PathPoints.Num() > 1)
		ObjectifLocalDir = path->PathPoints[1];
	else
		ObjectifLocalDir = ObjectifLocation;

	ObjectifLocalDir.Z = SoldierLocation.Z;

	DrawDebugPoint(GetWorld(), ObjectifLocalDir, 10, FColor::Red);

	ObjectifVector = ObjectifLocalDir - SoldierLocation;
	ObjectifLocation.Z = SoldierLocation.Z;
	ObjectifVector = ObjectifVector.GetSafeNormal(DefaultNormalizeVectorTolerance);
}

void AAIBasicController::UpdateMovementVector()
{
	MovementVector = AlignementVector * AlignementWeight
		+ CohesionVector * CohesionWeight
		+ SeparationVector * SeparationWeight
		+ ObjectifVector * ObjectifWeight;
	MovementVector = MovementVector.GetSafeNormal(DefaultNormalizeVectorTolerance);
}

void AAIBasicController::DrawDebug()
{
	FVector SoldierLocation = GetPawn()->GetActorLocation();
	SoldierLocation.Z += 100;
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + AlignementVector * AlignementWeight, FColor::Green); /*Alignement vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + CohesionVector * CohesionWeight, FColor::Blue); /*Cohesion vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + SeparationVector * SeparationWeight, FColor::Red); /*Separation vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + ObjectifVector  * ObjectifWeight, FColor::Yellow); /*Objectif vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + MovementVector, FColor::Black); /*Movement vector*/
	DrawDebugPoint(GetWorld(), ObjectifLocation, 12, FColor::Purple);
	DrawDebugPoint(GetWorld(), blackboard->GetValueAsVector("FlockingLocation"), 12, FColor::Black);
}

void AAIBasicController::UpdateFlockingPosition(float DeltaSeconds)
{
	ResetVectors();

	UpdateNeighbourhood();

	UpdateAlignementVector();

	if (SeenBoids.Num() > 0) { //because this involve /0
		UpdateCohesionVector();

		UpdateSeparationVector();
	}

	UpdateObjectifVector();

	UpdateMovementVector();

	float MaxSpeed = GetPawn()->GetMovementComponent()->GetMaxSpeed();
	MovementVector = MovementVector * MaxSpeed;
	blackboard->SetValueAsVector("FlockingLocation", GetPawn()->GetActorLocation() + MovementVector);

	DrawDebug();
}

EPathFollowingRequestResult::Type AAIBasicController::FollowFlocking() {
	EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(blackboard->GetValueAsVector("FlockingLocation"), 5.f);
	return _movetoResult;
}


void AAIBasicController::setup_BehaviorTree() {
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/BT_AIBasic.BT_AIBasic'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}

void AAIBasicController::Tick(float DeltaSeconds) {
	UpdateFlockingPosition(DeltaSeconds);
}
