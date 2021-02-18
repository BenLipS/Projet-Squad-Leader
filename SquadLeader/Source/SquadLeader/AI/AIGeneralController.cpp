// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGeneralController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Soldiers/Soldier.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "../Soldiers/AIs/SoldierAI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

AAIGeneralController::AAIGeneralController(FObjectInitializer const& object_initializer)
{
	setup_BehaviorTree();
	setup_perception_system();
	m_destination = FVector(0.f, 0.f, 0.f);
}

void AAIGeneralController::BeginPlay() {
	Super::BeginPlay();
	RunBehaviorTree(m_behaviorTree);
	blackboard = BrainComponent->GetBlackboardComponent();
}

void AAIGeneralController::ontargetperception_update_sight(AActor* actor, FAIStimulus stimulus) {
	//if (GEngine)GEngine->AddOnScreenDebugMessage(5961, 1.f, FColor::Blue, TEXT("ontargetperception_update_sight"));
};

void AAIGeneralController::ActorsPerceptionUpdated(const TArray < AActor* >& UpdatedActors) {
	for (auto& Elem : UpdatedActors) {
		if (SeenActor.Contains(Elem));
		else SeenActor.Add(Elem);
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
		sight_config->LoseSightRadius = sight_config->SightRadius + 200.0f;
		sight_config->PeripheralVisionAngleDegrees = 82.5f;
		sight_config->SetMaxAge(1.f);
		sight_config->AutoSuccessRangeFromLastSeenLocation = 100.0f;
		sight_config->DetectionByAffiliation.bDetectEnemies = true;
		sight_config->DetectionByAffiliation.bDetectFriendlies = true;
		sight_config->DetectionByAffiliation.bDetectNeutrals = true;

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
	UBlackboardComponent* BlackboardComponent = BrainComponent->GetBlackboardComponent();
	
	if (BlackboardComponent->GetValueAsBool("is_attacking"))
		return EPathFollowingRequestResult::Type::Failed;

	//TO-DO : if follow an enemy be at the distance to shoot 
	EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(m_destination, 500.f);
	if(_movetoResult == EPathFollowingRequestResult::Type::AlreadyAtGoal)
		BlackboardComponent->ClearValue("VectorLocation");
	
	return _movetoResult;
}

EPathFollowingRequestResult::Type AAIGeneralController::MoveToEnemyLocation() {
	ASoldier* _soldier_enemy = Cast<ASoldier>(blackboard->GetValueAsObject("FocusActor"));

	ASoldierAI* _soldier = Cast<ASoldierAI>(GetPawn());
	if (_soldier_enemy) {
		if (_soldier) {
			UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), GetPawn()->GetActorLocation(), _soldier_enemy->GetActorLocation(), NULL);

			if (path->GetPathLength() >= m_distanceShootAndWalk)
				_soldier->ActivateAbilityRun();
			else
				_soldier->CancelAbilityRun();
		}

		//TO-DO : if follow an enemy be at the distance to shoot 
		EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(_soldier_enemy->GetActorLocation(), m_distanceShootAndStop);
		if (_movetoResult == EPathFollowingRequestResult::Type::AlreadyAtGoal)
			blackboard->ClearValue("VectorLocation");
		return _movetoResult;
	}
	return EPathFollowingRequestResult::Failed;
}

void AAIGeneralController::ShootEnemy() {
	if (GEngine) GEngine->AddOnScreenDebugMessage(10, 1.f, FColor::Red, TEXT("I shoot !"));

	if (ASoldierAI* soldier = Cast<ASoldierAI>(GetPawn()); soldier)
	{
		soldier->ActivateAbilityFire();
		soldier->CancelAbilityFire();
	}
}

void AAIGeneralController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	Sens();
	Think(); // == if we need to change the BehaviorTree,
	//Act will be done in the behavior tree

	
}

void AAIGeneralController::Sens() {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(10, 1.f, FColor::Yellow, TEXT("Sens !!"));

	/* Update Seen Actors/ Delete hidden Actors*/
	TArray<AActor*> ActorToRemove;
	for (auto& Elem : SeenActor) {
		FActorPerceptionBlueprintInfo info;
		GetPerceptionComponent()->GetActorsPerception(Elem, info);
		if (info.LastSensedStimuli.Last().IsExpired()) {
			ActorToRemove.Add(Elem);
		}
	}
	for (auto& Elem : ActorToRemove) {
		SeenActor.Remove(Elem);
	}

	//ClearFocus(EAIFocusPriority::Gameplay);
	blackboard->ClearValue("FocusActor");
	if (SeenActor.Num() > 0) {
		this->SetFocalPoint(SeenActor[0]->GetTargetLocation());
		blackboard->SetValueAsObject("FocusActor", SeenActor[0]);
	}
	else {
		ASoldierAI* _solider = Cast<ASoldierAI>(GetPawn());
		_solider->CancelAbilityRun();
	}
}

void AAIGeneralController::Think() {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(20, 1.f, FColor::Purple, TEXT("Think !!"));

	if (blackboard->GetValueAsObject("FocusActor")) {
		//Attack Comportment
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(21, 1.f, FColor::Purple, TEXT("In Attack mode"));
		m_behavior = AIBehavior::Attack;
		blackboard->SetValueAsBool("is_attacking", true);
	}
	else {
		//Defens comportment
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(21, 1.f, FColor::Purple, TEXT("In Defensiv mode"));
		m_behavior = AIBehavior::Defense;
		blackboard->SetValueAsBool("is_attacking", false);
	}
}


UBlackboardComponent* AAIGeneralController::get_blackboard() const
{
	return blackboard;
}