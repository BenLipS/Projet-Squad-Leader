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

void AAISquadController::BeginPlay()
{
	Super::BeginPlay();
	blackboard->SetValueAsBool("IsInFormation", true);
	blackboard->SetValueAsBool("HasOrder", false);
}

void AAISquadController::Init()
{

}

EPathFollowingRequestResult::Type AAISquadController::FollowFormation() {
	GetValidFormationPos();
	EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(blackboard->GetValueAsVector("FormationLocation"), 50.f);
	DrawDebugPoint(GetWorld(), blackboard->GetValueAsVector("FormationLocation"), 12, FColor::Purple);
	if ((blackboard->GetValueAsVector("FormationLocation") - GetPawn()->GetActorLocation()).Size() >= RuningDistanceForFormation && !RunToFormation) {
		Cast<ASoldierAI>(GetPawn())->ActivateAbilityRun();
		RunToFormation = true;
	}
	else if ((blackboard->GetValueAsVector("FormationLocation") - GetPawn()->GetActorLocation()).Size() < StopRuningDistanceForFormation) {
		DrawDebugPoint(GetWorld(), GetPawn()->GetActorLocation(), 32, FColor::Red);
		Cast<ASoldierAI>(GetPawn())->CancelAbilityRun();
		RunToFormation = false;
	}

	return _movetoResult;
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
	SetState(AIBasicState::Moving);
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_moving", false);
	blackboard->SetValueAsBool("is_patroling", false);
	blackboard->SetValueAsBool("is_searching", false);
	blackboard->SetValueAsBool("IsInFormation", true);
}