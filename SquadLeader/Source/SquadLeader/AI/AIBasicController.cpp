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


AAIBasicController::AAIBasicController()
{
	setup_BehaviorTree();
}


void AAIBasicController::BeginPlay() {
	Super::BeginPlay();
	Cast<USquadLeaderGameInstance>(GetGameInstance())->AddAIBasicToManager(this);
}

void AAIBasicController::UpdateMission()
{
	if (Mission) {
		if (Mission->Type == MissionType::MoveTo)
			ObjectifLocation = Mission->Location;
	}
}

void AAIBasicController::setup_BehaviorTree() {
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/AIBasic/BT_AIBasic.BT_AIBasic'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}

void AAIBasicController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	UpdateMission();
}

FVector AAIBasicController::GetRespawnPoint()  // TODO : Change this function to adapt the squad AI respawn
{
	if (ASoldier* soldier = Cast<ASoldier>(GetPawn()); soldier) {
		if (soldier->GetTeam()) {
			auto AvailableSpawnPoints = soldier->GetTeam()->GetUsableSpawnPoints();
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

void AAIBasicController::Die() {
	Super::Die();
}

void AAIBasicController::ResetBlackBoard() {
	Super::ResetBlackBoard();
	blackboard->SetValueAsBool("DoFlocking", true);
	blackboard->SetValueAsVector("FlockingLocation", Cast<ASoldierAI>(GetPawn())->GetLocation());
}