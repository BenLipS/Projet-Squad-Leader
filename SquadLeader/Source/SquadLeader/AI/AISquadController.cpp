// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquadController.h"
#include "../Spawn/SoldierSpawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AAISquadController::AAISquadController() {
	setup_BehaviorTree();
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
			auto AvailableSpawnPoints = soldier->GetTeam().GetDefaultObject()->GetUsableSpawnPoints();
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
	EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(blackboard->GetValueAsVector("FormationLocation"), 5.f);

	if ((blackboard->GetValueAsVector("FormationLocation") - GetPawn()->GetActorLocation()).Size() >= RuningDistanceForFormation)
		Cast<ASoldierAI>(GetPawn())->ActivateAbilityRun();
	else
		Cast<ASoldierAI>(GetPawn())->CancelAbilityRun();

	return _movetoResult;
}

void AAISquadController::Die() {
	Super::Die();
}

void AAISquadController::ResetBlackBoard() const {
	Super::ResetBlackBoard();
	blackboard->SetValueAsVector("FormationLocation", Cast<ASoldier>(GetPawn())->GetLocation());
	blackboard->SetValueAsBool("HasOrder", false);
	blackboard->SetValueAsBool("IsInFormation", true);
}
