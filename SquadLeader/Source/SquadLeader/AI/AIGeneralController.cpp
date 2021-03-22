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


AAIGeneralController::AAIGeneralController(FObjectInitializer const& object_initializer)
{
	setup_BehaviorTree();
	setup_perception_system();
	//m_destination = FVector(11410.f, 2950.f, 0.f);
	m_destination = FVector(5000.f, 5000.f, 0.f);
}

void AAIGeneralController::BeginPlay() {
	Super::BeginPlay();

	FlockingComponent = NewObject<UFlockingComponent>(this, ClassFlockingComponent);
	RunBehaviorTree(m_behaviorTree);
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
	FlockingComponent->UpdateFlockingPosition(DeltaSeconds);
	//Act will also be done in the behavior tree

}

void AAIGeneralController::Sens() {
	UpdateSeenSoldier();
	FocusEnemy();
}

void AAIGeneralController::Think() {
	ChooseState();
}

void AAIGeneralController::Act() {
	switch (m_state) {
	case AIBasicState::Attacking:
		TooClose();
		TooFar();
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
}
void AAIGeneralController::PatrolingState() {
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_patroling", true);
	blackboard->SetValueAsBool("is_moving", false);
	blackboard->SetValueAsBool("is_searching", false);
}
void AAIGeneralController::MovingState() {
	//Check if it's new or not
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_moving", true);
	blackboard->SetValueAsBool("is_patroling", false);
	blackboard->SetValueAsBool("is_searching", false);
}
void AAIGeneralController::SearchState() {
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_moving", false);
	blackboard->SetValueAsBool("is_patroling", false);
	blackboard->SetValueAsBool("is_searching", true);
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
void AAIGeneralController::TooFar() {
	ASoldier* _FocusEnemy = Cast<ASoldier>(blackboard->GetValueAsObject("FocusActor"));
	float _distance = FVector::Dist(GetPawn()->GetActorLocation(), _FocusEnemy->GetActorLocation());

	if (_distance > m_distanceShootAndStop + 100.f) {
		blackboard->SetValueAsBool("need_GoForward", true);
	}
	else {
		blackboard->SetValueAsBool("need_GoForward", false);
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

void AAIGeneralController::SetMission(UMission* _Mission)
{
	Mission = _Mission;
}

UMission* AAIGeneralController::GetMission()
{
	return Mission;
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

void AAIGeneralController::ResetBlackBoard() const
{
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("is_moving", true);
	blackboard->SetValueAsBool("is_patroling", false);
	blackboard->SetValueAsBool("is_searching", false);
	blackboard->SetValueAsBool("need_GoBackward", false);
	blackboard->SetValueAsBool("need_GoForward", false);
	blackboard->SetValueAsObject("FocusActor", NULL);
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
	PatrolPos.X = FMath::FRandRange(0, 1600);
	PatrolPos.Y = FMath::FRandRange(0, 1600);
	PatrolPos.Z = FMath::FRandRange(0, 1600);

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