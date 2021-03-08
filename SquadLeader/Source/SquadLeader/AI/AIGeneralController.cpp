// Fill out your copyright notice in the Description page of Project Settings.


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


AAIGeneralController::AAIGeneralController(FObjectInitializer const& object_initializer)
{
	setup_BehaviorTree();
	setup_perception_system();
	//m_destination = FVector(11410.f, 2950.f, 0.f);
	m_destination = FVector(5000.f, 5000.f, 0.f);
}

void AAIGeneralController::BeginPlay() {
	Super::BeginPlay();
	RunBehaviorTree(m_behaviorTree);
	blackboard = BrainComponent->GetBlackboardComponent();
	blackboard->SetValueAsBool("IsHit", false);
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

	for (auto& Elem : UpdatedActors) {
		if(ASoldier* soldier = Cast<ASoldier>(Elem); soldier && soldier->IsAlive()){
			if (SeenSoldier.Contains(soldier));
			else SeenSoldier.Add(soldier);
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

EPathFollowingRequestResult::Type AAIGeneralController::MoveToActorLocation() {
	AActor* _actor = Cast<AActor>(blackboard->GetValueAsObject("ActorLocation"));
	EPathFollowingRequestResult::Type _movetoResult = MoveToActor(_actor);

	return _movetoResult;
}

EPathFollowingRequestResult::Type AAIGeneralController::MoveToVectorLocation() {
	
	if (blackboard->GetValueAsBool("is_attacking") && !blackboard->GetValueAsBool("need_GoBackward"))
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
		EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(blackboard->GetValueAsVector("EnemyLocation"), m_distanceShootAndStop);
		if (_movetoResult == EPathFollowingRequestResult::AlreadyAtGoal)
			blackboard->ClearValue("EnemyLocation");
		return _movetoResult;
	}
	return EPathFollowingRequestResult::Failed;
}

ResultState AAIGeneralController::ShootEnemy() {
	if (GEngine) GEngine->AddOnScreenDebugMessage(10, 1.f, FColor::Red, TEXT("I shoot !"));

	if (ASoldierAI* soldier = Cast<ASoldierAI>(GetPawn()); soldier && GetFocusActor())
	{
		soldier->SetLookingAtPosition(GetFocusActor()->GetTargetLocation());
		soldier->ActivateAbilityFire();
		soldier->CancelAbilityFire();
		if (auto _solider = Cast<ASoldier>(GetFocusActor()); !_solider->IsAlive()) {
			blackboard->SetValueAsObject("FocusActor", NULL);
			return ResultState::Success;
		}
		return ResultState::InProgress;
	}
	return ResultState::Failed;
}

void AAIGeneralController::Tick(float DeltaSeconds) {
	Sens();
	Think(); // == if we need to change the BehaviorTree,
	Act();
	//Act will also be done in the behavior tree
	Super::Tick(DeltaSeconds);
}

void AAIGeneralController::Sens() {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(10, 1.f, FColor::Yellow, TEXT("Sens !!"));
	UpdateSeenSoldier();
}

void AAIGeneralController::Think() {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(20, 1.f, FColor::Purple, TEXT("Think !!"));
	ChooseBehavior();
	if(AIBehavior::Attack == m_behavior){
		//see if in a good range
		TooFar();
		TooClose();
	}
}

void AAIGeneralController::Act() {
	FocusEnemy();
}

UBlackboardComponent* AAIGeneralController::get_blackboard() const
{
	return blackboard;
}

void AAIGeneralController::ChooseBehavior() {
	if (blackboard->GetValueAsObject("FocusActor")) {
		AttackBehavior();
	}
	else {
		DefenseBehavior();
	}
}

void AAIGeneralController::FocusEnemy() {
	ClearFocus(EAIFocusPriority::Gameplay);
	blackboard->ClearValue("FocusActor");
	if (SeenSoldier.Num() > 0) {
		bool enemyDetected = false;
		int i = 0;
		while (!enemyDetected && i < SeenSoldier.Num()) {
			if (Cast<ASoldier>(SeenSoldier[i])->GetTeam() != Cast<ASoldier>(GetPawn())->GetTeam()) {
				this->SetFocus(SeenSoldier[i]);
				blackboard->SetValueAsObject("FocusActor", SeenSoldier[i]);
				enemyDetected = true;
			}
			i++;
		}
	}
	else {
		ASoldierAI* _solider = Cast<ASoldierAI>(GetPawn());
		_solider->CancelAbilityRun();
	}
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
		ASoldier* _enemy = Cast<ASoldier>(blackboard->GetValueAsObject("FocusActor"));
		blackboard->SetValueAsVector("EnemyLocation", _enemy->GetActorLocation());
	}
	else {
		blackboard->SetValueAsBool("need_GoForward", false);
		blackboard->ClearValue("EnemyLocation");
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

void AAIGeneralController::AttackBehavior() {
	//Attack Comportment
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(21, 1.f, FColor::Purple, TEXT("In Attack mode"));
	if (m_behavior == AIBehavior::Defense) {
		m_behavior = AIBehavior::Attack;
		blackboard->SetValueAsBool("is_attacking", true);
		m_state = AIBasicState::Attacking;
		blackboard->SetValueAsBool("DoFlocking", false);
		ASoldier* _enemy = Cast<ASoldier>(blackboard->GetValueAsObject("FocusActor"));
		blackboard->SetValueAsVector("EnemyLocation", _enemy->GetActorLocation());
	}
}
void AAIGeneralController::DefenseBehavior() {
	//Defens comportment
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(21, 1.f, FColor::Purple, TEXT("In Defensiv mode"));
	//Check if it's new or not
	if (m_behavior == AIBehavior::Attack) {
		m_behavior = AIBehavior::Defense;
		blackboard->SetValueAsBool("is_attacking", false);
		blackboard->SetValueAsBool("DoFlocking", true);
		m_state = AIBasicState::Moving;
		}
}

void AAIGeneralController::SetMission(UMission* _Mission)
{
	Mission = _Mission;
	if (blackboard) {
		blackboard->SetValueAsVector("MissionLocation", Mission->Location);
	}
}
	

UMission* AAIGeneralController::GetMission()
{
	return Mission;
}

void AAIGeneralController::Die() const {
	ResetBlackBoard();
}

void AAIGeneralController::ResetBlackBoard() const
{
	blackboard->SetValueAsBool("is_attacking", false);
	blackboard->SetValueAsBool("need_GoBackward", false);
	blackboard->SetValueAsBool("need_GoForward", false);
	blackboard->SetValueAsBool("IsSearching", false);
	blackboard->SetValueAsBool("IsHit", false);
	blackboard->SetValueAsObject("FocusActor", NULL);
	blackboard->SetValueAsVector("EnemyLocation", FVector());
	blackboard->SetValueAsVector("VectorLocation", FVector());
	blackboard->SetValueAsVector("MissionLocation", FVector());
}

void AAIGeneralController::SetState(AIBasicState _state) noexcept {
	m_state = _state;
}