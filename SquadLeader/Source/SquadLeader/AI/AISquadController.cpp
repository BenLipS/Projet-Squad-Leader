// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquadController.h"
#include "AISquadManager.h"
#include "../Spawn/SoldierSpawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "NavigationSystem.h"

bool AAISquadController::GetValidFormationPos()
{
	FVector HitLocation{};

	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	FVector startLocation = SquadManager->Leader->GetLocation();
	FVector endLocation = blackboard->GetValueAsVector("FormationLocation");

	if (navSys->NavigationRaycast(GetWorld(), startLocation, endLocation, HitLocation)) {
		blackboard->SetValueAsVector("FormationLocation", HitLocation);
		return false;
	}
	return true;
}

AAISquadController::AAISquadController() {
	setup_BehaviorTree();
	bReplicates = false;
}

void AAISquadController::setup_BehaviorTree() {
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/BT_AISquad.BT_AISquad'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}

FVector AAISquadController::GetRespawnPoint()  // TODO : Change this function to adapt the squad AI respawn
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

void AAISquadController::BeginPlay()
{
	Super::BeginPlay();
	blackboard->SetValueAsBool("IsInFormation", true);
	blackboard->SetValueAsBool("HasOrder", false);
}

void AAISquadController::Init()
{
	Super::Init();
	SetState(AIBasicState::Formation);
	FormationState();
}

void AAISquadController::FollowFormation() {
	GetValidFormationPos();
	//EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(blackboard->GetValueAsVector("FormationLocation"), 5.f);
	//DrawDebugPoint(GetWorld(), blackboard->GetValueAsVector("FormationLocation"), 12, FColor::Purple);
}

void AAISquadController::Die() {
	Super::Die();
}

void AAISquadController::ResetBlackBoard() {
	Super::ResetBlackBoard();
	blackboard->SetValueAsVector("FormationLocation", Cast<ASoldier>(GetPawn())->GetLocation());
	blackboard->SetValueAsBool("HasOrder", false);
	blackboard->SetValueAsBool("IsInFormation", true);
}

void AAISquadController::FormationState() {
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_moving", false);
	blackboard->SetValueAsBool("is_patroling", false);
	blackboard->SetValueAsBool("is_searching", false);
	blackboard->SetValueAsBool("is_capturing", false);
	blackboard->SetValueAsBool("IsInFormation", true);
}

void AAISquadController::UpdateFormation(const FVector _position) {
	get_blackboard()->SetValueAsVector("FormationLocation", _position);
}

void AAISquadController::SetUpMission(bool hasOrder, bool isInFormation, FVector _Location){
	StopCurrentBehavior = true;
	get_blackboard()->SetValueAsBool("HasOrder", hasOrder);
	get_blackboard()->SetValueAsBool("IsInFormation", isInFormation);
	if (isInFormation) {
		SetState(AIBasicState::Formation);
	}
	else {
		SetState(AIBasicState::Moving);
	}
}

void AAISquadController::ChooseState() {
	if (m_state == AIBasicState::Attacking) {
		AttackingState();
	}
	else if (m_state == AIBasicState::Capturing) {
		CapturingState();
	}
	else if (m_state == AIBasicState::Patroling) {
		PatrolingState();
	}
	else if (m_state == AIBasicState::Search) {
		SearchState();
	}
	else if (m_state == AIBasicState::Formation) {
		FormationState();
	}
	else {
		MovingState();
	}
}