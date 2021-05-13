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

	FVector startLocation = UNavigationSystemV1::ProjectPointToNavigation(GetWorld(), SquadManager->Leader->GetLocation());
	
	FVector endLocation = FormationPosBeforeTransform;

	blackboard->SetValueAsVector("FormationLocation", FormationPosBeforeTransform);

	if (navSys->NavigationRaycast(GetWorld(), startLocation, endLocation, HitLocation)) {
		blackboard->SetValueAsVector("FormationLocation", HitLocation);
		if ((HitLocation - SquadManager->Leader->GetLocation()).Size() < 100.f)//if too close to player stop movement
			StopMovement();
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
	if (ASoldierPlayer* leader = SquadManager->Leader; leader) {
		if (leader->GetTeam()) {
			auto AvailableSpawnPoints = leader->GetTeam()->GetUsableSpawnPoints();
			if (AvailableSpawnPoints.Num() > 0) {

				ASoldierSpawn* OptimalSpawn = AvailableSpawnPoints[0];
				auto CalculateMinimalDistance = [](FVector PlayerPos, FVector FirstPoint, FVector SecondPoint) {  // return true if the first point is closest
					float dist1 = FVector::Dist(PlayerPos, FirstPoint);
					float dist2 = FVector::Dist(PlayerPos, SecondPoint);
					return dist1 < dist2;
				};

				for (auto loop : AvailableSpawnPoints) {
					if (CalculateMinimalDistance(leader->GetActorLocation(), loop->GetActorLocation(), OptimalSpawn->GetActorLocation())) {
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

void AAISquadController::Tick(float DeltaSecond) {
	HomeTick(DeltaSecond);
}

void AAISquadController::Init()
{
	Super::Init();
	SetState(AIBasicState::Formation);
	FormationState();
}

void AAISquadController::FollowFormation() {
	if (SquadManager->Leader) {
		GetValidFormationPos();
		//EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(blackboard->GetValueAsVector("FormationLocation"), 5.f);
	}
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

void AAISquadController::SetState(AIBasicState _state) noexcept
{
	Super::SetState(_state);
	if(IsValid(SquadManager))
		SquadManager->OnSquadMemberMissionChange(m_state, this);
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
	FormationPosBeforeTransform = _position;
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

void AAISquadController::BecomeAvailable() {
	return;
}
void AAISquadController::BecomeUnavailable() {
	return;
}