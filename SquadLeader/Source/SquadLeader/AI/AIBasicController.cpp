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
#include "../SquadLeaderGameInstance.h"
#include "../Spawn/SoldierSpawn.h"
#include "Mission.h"


AAIBasicController::AAIBasicController() :
	AlignementWeight{ 1.f },
	CohesionWeight{ 1.f },
	SeparationWeight{ 2.f },
	ObjectifWeight{ 2.5f },
	MovementVectorScale{ 2.f },
	SeparationVector{ 0.f, 0.f, 0.f },
	CohesionVector{ 0.f, 0.f, 0.f },
	AlignementVector{ 0.f, 0.f, 0.f },
	ObjectifVector{ 0.f, 0.f, 0.f },
	MovementVector{ 0.f, 0.f, 0.f },
	ObjectifLocation{ 200.f, 0.f, 10.f }
{
	setup_BehaviorTree();
}


void AAIBasicController::BeginPlay() {
	Super::BeginPlay();
	//Cast<USquadLeaderGameInstance>(GetGameInstance())->AddAIBasicToManager(this);
	blackboard->SetValueAsBool("DoFlocking", false);
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
		if (AAIBasicController* AIBasic = Cast<AAIBasicController>(soldier->Controller); AIBasic && soldier->PlayerTeam == Cast<ASoldier>(GetPawn())->PlayerTeam)
			SeenBoids.Add(AIBasic);
	};
}

void AAIBasicController::UpdateCohesionVector()
{
	FVector SoldierLocation = GetPawn()->GetActorLocation();
	for (AAIBasicController* Boid : SeenBoids)
	{
		/* Cohesion throught pathfinding and not absolute*/
		UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), SoldierLocation, Boid->GetPawn()->GetActorLocation(), NULL);

		FVector CohesionLocalDir;
		if (path->PathPoints.Num() > 2)
			CohesionLocalDir = path->PathPoints[1];
		else
			CohesionLocalDir = Boid->GetPawn()->GetActorLocation();

		//DrawDebugPoint(GetWorld(), CohesionLocalDir, 10, FColor::Red);
		
		CohesionVector += CohesionLocalDir.GetSafeNormal(DefaultNormalizeVectorTolerance) * Boid->GetPawn()->GetActorLocation().Size() - SoldierLocation;
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

	//DrawDebugPoint(GetWorld(), ObjectifLocalDir, 10, FColor::Red);

	ObjectifVector = ObjectifLocalDir - SoldierLocation;
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
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + AlignementVector * AlignementWeight * 100, FColor::Green); /*Alignement vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + CohesionVector * CohesionWeight * 100, FColor::Blue); /*Cohesion vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + SeparationVector * SeparationWeight * 100, FColor::Red); /*Separation vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + ObjectifVector  * ObjectifWeight * 100, FColor::Yellow); /*Objectif vector*/
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
	MovementVector = MovementVector * MaxSpeed/2;
	blackboard->SetValueAsVector("FlockingLocation", GetPawn()->GetActorLocation() + MovementVector);

	//DrawDebug();
}

void AAIBasicController::UpdateMission()
{
	if (Mission) {
		if (Mission->Type == MissionType::MoveTo)
			ObjectifLocation = Mission->Location;
	}
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
	Super::Tick(DeltaSeconds);
	UpdateMission();
	UpdateFlockingPosition(DeltaSeconds);
}

FVector AAIBasicController::GetRespawnPoint()  // TODO : Change this function to adapt the squad AI respawn
{
	if (ASoldier* soldier = Cast<ASoldier>(GetPawn()); soldier) {
		if (soldier->PlayerTeam) {
			auto AvailableSpawnPoints = soldier->PlayerTeam.GetDefaultObject()->GetUsableSpawnPoints();
			if (AvailableSpawnPoints.Num() > 0) {

				FVector OptimalPosition = AvailableSpawnPoints[0]->GetActorLocation();
				auto CalculateMinimalDistance = [](FVector PlayerPos, FVector FirstPoint, FVector SecondPoint) {  // return true if the first point is closest
					float dist1 = FVector::Dist(PlayerPos, FirstPoint);
					float dist2 = FVector::Dist(PlayerPos, SecondPoint);
					return dist1 < dist2;
				};

				for (auto loop : AvailableSpawnPoints) {
					if (CalculateMinimalDistance(soldier->GetActorLocation(), loop->GetActorLocation(), OptimalPosition)) {
						OptimalPosition = loop->GetActorLocation();
					}
				}

				return OptimalPosition;
			}
		}
	}
	return FVector(0.f, 0.f, 1500.f); // else return default
}
