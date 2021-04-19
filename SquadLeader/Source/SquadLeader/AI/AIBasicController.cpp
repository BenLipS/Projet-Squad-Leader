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
#include "../SquadLeaderGameModeBase.h"
#include "../Spawn/SoldierSpawn.h"
#include "Mission.h"
#include "AIBasicManager.h"

AAIBasicController::AAIBasicController()
{
	setup_BehaviorTree();
}

void AAIBasicController::BeginPlay() {
	Super::BeginPlay();
	Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode())->AddAIBasicToManager(this);
}

void AAIBasicController::Init() {
	Super::Init();

	m_state = AIBasicState::Patroling;
	m_old_state = m_state;
	blackboard->SetValueAsBool("is_patroling", true);
	blackboard->SetValueAsVector("VectorLocation", GetPawn()->GetActorLocation());
}

void AAIBasicController::setup_BehaviorTree() {
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/AIBasic/BT_AIBasic.BT_AIBasic'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}

void AAIBasicController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}

FVector AAIBasicController::GetRespawnPoint()  // TODO : Change this function to adapt the squad AI respawn
{
	if (ASoldier* soldier = Cast<ASoldier>(GetPawn()); soldier) {
		if (soldier->GetTeam()) {
			auto AvailableSpawnPoints = soldier->GetTeam()->GetUsableSpawnPoints();
			if (AvailableSpawnPoints.Num() > 0) {

				ASoldierSpawn* OptimalSpawn = AvailableSpawnPoints[0];
				auto CalculateMinimalDistance = [](FVector PlayerPos, FVector FirstPoint, FVector SecondPoint) {  // return true if the first point is closest
					float dist1 = FVector::Dist(PlayerPos, FirstPoint);
					float dist2 = FVector::Dist(PlayerPos, SecondPoint);
					return dist1 < dist2;
				};

				for (auto loop : AvailableSpawnPoints) {
					if (CalculateMinimalDistance(soldier->GetActorLocation(), loop->GetActorLocation(), OptimalSpawn->GetActorLocation())) {
						OptimalSpawn = loop;
					}
				}

				return OptimalSpawn->GetSpawnLocation();
			}
		}
	}
	return FVector(200.f, 200.f, 1500.f); // else return default
}

void AAIBasicController::Die() {
	Super::Die();
}

void AAIBasicController::ResetBlackBoard() {
	Super::ResetBlackBoard();
	blackboard->SetValueAsBool("DoFlocking", true);
	blackboard->SetValueAsVector("FlockingLocation", Cast<ASoldierAI>(GetPawn())->GetLocation());
}

void AAIBasicController::SetManager(AAIBasicManager* _manager) noexcept {
	m_manager = _manager;
}