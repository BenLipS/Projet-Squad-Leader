// Fill out your copyright notice in the Description page of Project Settings.
/*
* Things to know : 
*  - we have a value m_state and m_old_state. This two value are, most of the time, the same.
* It only differ when the AI enter in the Attacking state, because after the attack, we want the AI to return to the state he was.
* For now this is the best solution, maybe we'll find a better way to do this
*/

#include "AIGeneralController.h"
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
#include "Components/FlockingComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/RandomStream.h"
#include "../ControlArea/ControlArea.h"



auto Fhome_variant::operator()(UCaptureMission* _mission)const
{
	GEngine->AddOnScreenDebugMessage(40, 10.f, FColor::Yellow, TEXT("Hello ! Mission de type UCaptureMission"));
	_mission->SetState(MissionState::eRUNNING);
	m_ai_controller->SetControlAreaBB(_mission->GetControlArea());
}
auto Fhome_variant::operator()(UDefendMission* _mission)const
{
	GEngine->AddOnScreenDebugMessage(40, 10.f, FColor::Yellow, TEXT("Mission de type UDefendMission"));
	_mission->SetState(MissionState::eRUNNING);
}
auto Fhome_variant::operator()(UPatrolMission* _mission)const
{
	GEngine->AddOnScreenDebugMessage(50, 10.f, FColor::Blue, TEXT("Mission de type UPatrolMission"));
	_mission->SetState(MissionState::eRUNNING);
	m_ai_controller->SetObjectifLocation(m_ai_controller->GetPawn()->GetActorLocation());
}

AAIGeneralController::AAIGeneralController(FObjectInitializer const& object_initializer)
{
	setup_BehaviorTree();
	setup_perception_system();
	//m_destination = FVector(11410.f, 2950.f, 0.f);
	m_destination = FVector(5000.f, 5000.f, 0.f);
	m_variant = Fhome_variant{};
	m_variant.m_ai_controller = this;
}

void AAIGeneralController::BeginPlay() {
	Super::BeginPlay();
	RunBehaviorTree(m_behaviorTree);
	Init();
	
	
	/*blackboard->SetValueAsObject("ControlArea", m_mission->GetControlArea());
	ObjectifLocation = m_mission->GetControlArea()->GetActorLocation();
	blackboard->SetValueAsVector("VectorLocation", ObjectifLocation);
	SetState(AIBasicState::Moving);*/
}

void AAIGeneralController::Init() {
	FlockingComponent = NewObject<UFlockingComponent>(this, ClassFlockingComponent);
	blackboard = BrainComponent->GetBlackboardComponent();

	m_state = AIBasicState::Moving;
	m_old_state = m_state;
	blackboard->SetValueAsBool("is_moving", true);

}

void AAIGeneralController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	Sens();
	Think(); // == if we need to change the BehaviorTree,
	Act();
	//Act will also be done in the behavior tree
	FlockingComponent->UpdateFlockingPosition(DeltaSeconds);

	CheckIfNeedToStopCurrentBehavior();
	//Act will also be done in the behavior tree

}

void AAIGeneralController::Sens() {
	UpdateSeenSoldier();
	FocusEnemy();
}

void AAIGeneralController::Think() {
	if (m_mission_changed) 
	{
		m_mission_changed = false;
		Visit(m_variant, m_mission_type);
	}
	ChooseState();
}

void AAIGeneralController::Act() {
	switch (m_state) {
	case AIBasicState::Attacking:
		TooClose();
		TooFar();
		break;
	case AIBasicState::Capturing:
		break;
	case AIBasicState::Patroling:
		break;
	case AIBasicState::Search:
		break;
	case AIBasicState::Moving:
		break;
	default:
		break;
	}
}

void AAIGeneralController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// everyone
	DOREPLIFETIME(AAIGeneralController, Team);
}

TSubclassOf<ASoldierTeam> AAIGeneralController::GetTeam()
{
	return Team;
}

bool AAIGeneralController::SetTeam(TSubclassOf<ASoldierTeam> _Team)
{
	if (GetLocalRole() == ROLE_Authority) {  // only server can change team
		if (Team)
			Team.GetDefaultObject()->RemoveSoldierList(Cast<ASoldier>(GetPawn()));
		if (_Team)
			_Team.GetDefaultObject()->AddSoldierList(Cast<ASoldier>(GetPawn()));

		Team = _Team;
		return true;
	}
	return false;
}

void AAIGeneralController::ontargetperception_update_sight(AActor* actor, FAIStimulus stimulus) {
	//if (GEngine)GEngine->AddOnScreenDebugMessage(5961, 1.f, FColor::Blue, TEXT("ontargetperception_update_sight"));
};

void AAIGeneralController::ActorsPerceptionUpdated(const TArray < AActor* >& UpdatedActors) {
	if (Cast<ASoldierAI>(GetPawn())->IsAlive()) {
		for (auto& Elem : UpdatedActors) {
			if (ASoldier* soldier = Cast<ASoldier>(Elem); soldier && soldier->IsAlive() && soldier->GetTeam().GetDefaultObject()->TeamName != "Spectator" && (this->GetPawn()->GetActorLocation() - Elem->GetActorLocation()).Size() < m_distancePerception){//TODO: Remove ugly last condition to avoid seig nearly respawned enemi , If team == spectateur then AI don't see you Cool to test
				if (SeenSoldier.Contains(soldier));
				else SeenSoldier.Add(soldier);
			}
		}
	}
	//if (GEngine)GEngine->AddOnScreenDebugMessage(5960, 1.f, FColor::Blue, TEXT("ActorsPerceptionUpdated"));
};

void AAIGeneralController::onperception_update_sight(const TArray<AActor*>& AArray) {
	//if (GEngine)GEngine->AddOnScreenDebugMessage(959, 1.f, FColor::Green, TEXT("I see: OnPerceptionUpdated"));
};

void AAIGeneralController::setup_perception_system() {
	// create and initialise sight configuration object
	sight_config = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (sight_config)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		sight_config->SightRadius = m_distancePerception;
		sight_config->LoseSightRadius = sight_config->SightRadius + m_LoseSightOffset;
		sight_config->PeripheralVisionAngleDegrees = m_PeripheralVisionAngleDegrees;
		sight_config->SetMaxAge(m_MaxAge);
		sight_config->AutoSuccessRangeFromLastSeenLocation = m_AutoSuccessRangeFromLastSeenLocation;
		sight_config->DetectionByAffiliation.bDetectEnemies = m_DetectEnemies;
		sight_config->DetectionByAffiliation.bDetectFriendlies = m_DetectFriendlies;
		sight_config->DetectionByAffiliation.bDetectNeutrals = m_DetectNeutrals;

		// add sight configuration component to perception component
		GetPerceptionComponent()->SetDominantSense(*sight_config->GetSenseImplementation());
		//GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAIGeneralController::ontargetperception_update_sight);
		//GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AAIGeneralController::onperception_update_sight);
		GetPerceptionComponent()->ConfigureSense(*sight_config);
	}
};

void AAIGeneralController::setup_BehaviorTree() {
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/BT_AIMainBehavior.BT_AIMainBehavior'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}

UBlackboardComponent* AAIGeneralController::get_blackboard() const
{
	return blackboard;
}

void AAIGeneralController::ChooseState() {
	if (m_state == AIBasicState::Attacking) {
		AttackingState();
	}
	else if (m_state == AIBasicState::Capturing) {
		CapturingState();
	}
	else if(m_state == AIBasicState::Patroling){
		PatrolingState();
	}
	else if (m_state == AIBasicState::Search) {
		SearchState();
	}
	else {
		MovingState();
	}
}

void AAIGeneralController::SetState(AIBasicState _state) noexcept {
	m_state = _state;
	m_old_state = m_state;
}

void AAIGeneralController::AttackingState() {
	//Attack Comportment
	blackboard->SetValueAsBool("is_moving", false);
	blackboard->SetValueAsBool("is_patroling", false);
	blackboard->SetValueAsBool("is_attacking", true);
	blackboard->SetValueAsBool("is_searching", false);
	blackboard->SetValueAsBool("is_capturing", false);
}
void AAIGeneralController::PatrolingState() {
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_patroling", true);
	blackboard->SetValueAsBool("is_moving", false);
	blackboard->SetValueAsBool("is_searching", false);
	blackboard->SetValueAsBool("is_capturing", false);
}
void AAIGeneralController::MovingState() {
	//Check if it's new or not
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_moving", true);
	blackboard->SetValueAsBool("is_patroling", false);
	blackboard->SetValueAsBool("is_searching", false);
	blackboard->SetValueAsBool("is_capturing", false);
}
void AAIGeneralController::SearchState() {
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_moving", false);
	blackboard->SetValueAsBool("is_patroling", false);
	blackboard->SetValueAsBool("is_capturing", true);
	blackboard->SetValueAsBool("is_searching", false);
}
void AAIGeneralController::CapturingState() {
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_moving", false);
	blackboard->SetValueAsBool("is_patroling", false);
	blackboard->SetValueAsBool("is_searching", false);
	blackboard->SetValueAsBool("is_capturing", true);
}

void AAIGeneralController::CheckIfNeedToStopCurrentBehavior()
{
	if (StopCurrentBehavior && !HasStopCurrentBehavior) {
		HasStopCurrentBehavior = true;
	}
	else if (HasStopCurrentBehavior) {
		HasStopCurrentBehavior = false;
		StopCurrentBehavior = false;
	}
}

void AAIGeneralController::FocusEnemy() {
	ClearFocus(EAIFocusPriority::Gameplay);
	blackboard->ClearValue("FocusActor");
	bool enemyDetected = false;
	int i = 0;
	if (SeenSoldier.Num() > 0) {
		while (!enemyDetected && i < SeenSoldier.Num()) {
			if (Cast<ASoldier>(SeenSoldier[i])->GetTeam() != Cast<ASoldier>(GetPawn())->GetTeam()) {
				this->SetFocus(SeenSoldier[i]);
				enemyDetected = true;
				//TO-DO : if already in the state attacking don't do this line
				m_state = AIBasicState::Attacking;
				blackboard->SetValueAsObject("FocusActor", SeenSoldier[i]);
				blackboard->SetValueAsVector("EnemyLocation", SeenSoldier[i]->GetActorLocation());
			}
			i++;
		}
		//TO-DO : the next four line are the basicly the same, see if we can find another way for doing this
		if (!enemyDetected && m_state != m_old_state)
			m_state = AIBasicState::Search;
	}else if(m_state != m_old_state)
		m_state = AIBasicState::Search;
}

void AAIGeneralController::Run(ASoldierAI* _soldier, ASoldier* _soldier_enemy) {
	if (_soldier) {
		UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), GetPawn()->GetActorLocation(), _soldier_enemy->GetActorLocation(), NULL);

		if (path->GetPathLength() >= m_distanceShootAndWalk)
			_soldier->ActivateAbilityRun();
		else
			_soldier->CancelAbilityRun();
	}
}

void AAIGeneralController::TooClose() {
	ASoldier* _FocusEnemy = Cast<ASoldier>(blackboard->GetValueAsObject("FocusActor"));
	if (_FocusEnemy) {
		float _distance = FVector::Dist(GetPawn()->GetActorLocation(), _FocusEnemy->GetActorLocation());

		if (_distance < m_distanceShootAndStop - 100.f) {
			blackboard->SetValueAsBool("need_GoBackward", true);
			FVector _DestinationToGo;
			float _d = m_distanceShootAndStop - _distance;
			FVector _unitaire = _FocusEnemy->GetActorForwardVector();
			_DestinationToGo = _unitaire * _d + GetPawn()->GetActorLocation();
			blackboard->SetValueAsVector("VectorLocation", _DestinationToGo);
		}
		else {
			blackboard->SetValueAsBool("need_GoBackward", false);
			blackboard->ClearValue("VectorLocation");
		}
	}
}
void AAIGeneralController::TooFar() {
	ASoldier* _FocusEnemy = Cast<ASoldier>(blackboard->GetValueAsObject("FocusActor"));
	if (_FocusEnemy) {
		float _distance = FVector::Dist(GetPawn()->GetActorLocation(), _FocusEnemy->GetActorLocation());

		if (_distance > m_distanceShootAndStop + 100.f) {
			blackboard->SetValueAsBool("need_GoForward", true);
		}
		else {
			blackboard->SetValueAsBool("need_GoForward", false);
		}
	}
}

void AAIGeneralController::UpdateSeenSoldier() {
	/* Update Seen Actors/ Delete hidden Actors*/
	TArray<ASoldier*> ActorToRemove;
	for (auto& Elem : SeenSoldier) {
		FActorPerceptionBlueprintInfo info;
		GetPerceptionComponent()->GetActorsPerception(Elem, info);
		if (info.Target == nullptr || info.LastSensedStimuli.Last().IsExpired() || !Cast<ASoldier>(Elem)->IsAlive()) {
			ActorToRemove.Add(Elem);
		}
	}
	for (auto& Elem : ActorToRemove) {
		SeenSoldier.Remove(Elem);
	}
}

template <class T>
void AAIGeneralController::SetMission(T _mission)
{
	//m_mission_type.Emplace<T>(_mission);
	m_mission_type.Set<T>(_mission);
	type_mission _mission_type{};
	_mission_type.Set<T>(_mission);
	m_missions.Add(_mission_type);

	m_mission_changed = true;
}

auto AAIGeneralController::GetMission()
{
	return 12;
}

void AAIGeneralController::Die() {
	//ResetBlackBoard();
	SeenSoldier.Empty();
	PerceptionComponent->ForgetAll();
}

void AAIGeneralController::Respawn() 
{
	ResetBlackBoard();
	//we restart the state machine
	//TO-DO : make a list of what should be in an Init function
	//for example SetState(AIBasicState::Moving) shall be in it
	//ResetBlackBoard() shall not
	SetState(AIBasicState::Moving);
	SeenSoldier.Empty();
	PerceptionComponent->ForgetAll();
}

void AAIGeneralController::ResetBlackBoard()
{
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_moving", true);
	blackboard->SetValueAsBool("is_patroling", false);
	blackboard->SetValueAsBool("is_searching", false);
	blackboard->SetValueAsBool("is_capturing", false);
	blackboard->SetValueAsBool("need_GoBackward", false);
	blackboard->SetValueAsBool("need_GoForward", false);
	blackboard->SetValueAsObject("FocusActor", NULL);
}

void AAIGeneralController::SetControlAreaBB(AControlArea* _controlArea) {
	blackboard->SetValueAsObject("ControlArea", _controlArea);
	ObjectifLocation = _controlArea->GetActorLocation();
	blackboard->SetValueAsVector("VectorLocation", ObjectifLocation);
	SetState(AIBasicState::Moving);
}

void AAIGeneralController::SetObjectifLocation(FVector _location) noexcept 
{
	ObjectifLocation = _location;
	blackboard->SetValueAsVector("VectorLocation", ObjectifLocation);
	SetState(AIBasicState::Moving);
}

/*
* 
* Function for Node in the behavior tree
* 
*/
EPathFollowingRequestResult::Type AAIGeneralController::MoveToActorLocation() {
	AActor* _actor = Cast<AActor>(blackboard->GetValueAsObject("ActorLocation"));
	EPathFollowingRequestResult::Type _movetoResult = MoveToActor(_actor);

	return _movetoResult;
}

EPathFollowingRequestResult::Type AAIGeneralController::MoveToVectorLocation() {
	
	if (m_state==AIBasicState::Attacking && !blackboard->GetValueAsBool("need_GoBackward"))
		return EPathFollowingRequestResult::Type::Failed;

	//TO-DO : if follow an enemy be at the distance to shoot 
	EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(blackboard->GetValueAsVector("VectorLocation"), 50.f);
	if (_movetoResult == EPathFollowingRequestResult::Type::AlreadyAtGoal) {
		SetState(AIBasicState::Patroling);
		blackboard->ClearValue("VectorLocation");
		blackboard->ClearValue("need_GoBackward");
	}
	
	return _movetoResult;
}

EPathFollowingRequestResult::Type AAIGeneralController::RunToVectorLocation(FVector Goal, float AcceptanceRadius) {

	EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(Goal, AcceptanceRadius);

	return _movetoResult;
}

EPathFollowingRequestResult::Type AAIGeneralController::MoveToEnemyLocation() {
	ASoldier* _soldier_enemy = Cast<ASoldier>(blackboard->GetValueAsObject("FocusActor"));

	ASoldierAI* _soldier = Cast<ASoldierAI>(GetPawn());
	if (_soldier_enemy) {
		Run(_soldier, _soldier_enemy);

		//TO-DO : if follow an enemy be at the distance to shoot 
		EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(_soldier_enemy->GetActorLocation(), m_distanceShootAndStop);

		return _movetoResult;
	}
	return EPathFollowingRequestResult::Failed;
}

EPathFollowingRequestResult::Type AAIGeneralController::MoveToSearchEnemy() {
	if (m_state != AIBasicState::Search)
		return EPathFollowingRequestResult::Failed;

	FVector location_ = blackboard->GetValueAsVector("EnemyLocation");

	ASoldierAI* soldier = Cast<ASoldierAI>(GetPawn());
	soldier->CancelAbilityRun();

	EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(location_, 5.f);

	return _movetoResult;
}

ResultState AAIGeneralController::ShootEnemy() {

	if (ASoldierAI* soldier = Cast<ASoldierAI>(GetPawn()); soldier && GetFocusActor() && m_state==AIBasicState::Attacking)
	{
		
		FVector ShootDir = GetFocusActor()->GetTargetLocation() - GetPawn()->GetActorLocation();

		/*Randomise the shoot*/
		FRandomStream ShootRand;
		ShootRand.GenerateNewSeed();
		FVector RandShootDir = ShootRand.VRandCone(ShootDir.GetSafeNormal(), HalfAngleShoot) * ShootDir.Size();

		soldier->SetLookingAtPosition(GetPawn()->GetActorLocation() + RandShootDir);
		soldier->ActivateAbilityFire();
		soldier->CancelAbilityFire();
		if (auto _solider = Cast<ASoldier>(GetFocusActor()); !_solider->IsAlive()) {
			m_state = m_old_state;
			return ResultState::Success;
		}
		return ResultState::InProgress;
	}
	return ResultState::Failed;
}

EPathFollowingRequestResult::Type AAIGeneralController::FollowFlocking() {
	EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(blackboard->GetValueAsVector("FlockingLocation"), 5.f);

	return _movetoResult;
}

void AAIGeneralController::SetPatrolPoint()
{
	FVector PatrolPos;
	PatrolPos.X = FMath::FRandRange(-HalfRadiusPatrol, HalfRadiusPatrol);
	PatrolPos.Y = FMath::FRandRange(-HalfRadiusPatrol, HalfRadiusPatrol);
	PatrolPos.Z = FMath::FRandRange(-HalfRadiusPatrol, HalfRadiusPatrol);

	FVector HitLocation{};

	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	FVector startLocation = ObjectifLocation;
	FVector endLocation = ObjectifLocation + PatrolPos;

	if (navSys->NavigationRaycast(GetWorld(), startLocation, endLocation, HitLocation))
		PatrolPos = HitLocation;

	blackboard->SetValueAsVector("PatrolPoint", ObjectifLocation + PatrolPos);
}

ResultState AAIGeneralController::ArriveAtDestination() {
	if ( GetPawn() && FVector::Dist(GetPawn()->GetActorLocation(), GetObjectifLocation()) < 300.f) {
		SetState(AIBasicState::Patroling);
		return ResultState::Success;
	}
	if (m_state == AIBasicState::Attacking)
		return ResultState::Failed;

	return ResultState::InProgress;
}

ResultState AAIGeneralController::EndTheResearch() {
	m_state = m_old_state;
	blackboard->ClearValue("EnemyLocation");
	return ResultState::Success;
}

ResultState AAIGeneralController::Capturing() {
	if (m_state != AIBasicState::Capturing)
		return ResultState::Failed;

	AControlArea* control_area = Cast<AControlArea>(blackboard->GetValueAsObject("ControlArea"));
	if (control_area) {
		if (auto value = control_area->TeamData.Find(Team)) {
			if ((*value)->controlValue >= control_area->maxControlValue) {
				SetState(AIBasicState::Patroling);
				//it'll depend of the mission
				//SetState(AIBasicState::Capturing);
				return ResultState::Success;
			}
			else
				return ResultState::InProgress;
		}
	}
	return ResultState::Failed;

}