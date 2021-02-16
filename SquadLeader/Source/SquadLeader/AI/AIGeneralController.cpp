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

AAIGeneralController::AAIGeneralController(FObjectInitializer const& object_initializer)
{
	setup_BehaviorTree();
	setup_perception_system();
	
}

void AAIGeneralController::BeginPlay() {
	Super::BeginPlay();
	RunBehaviorTree(m_behaviorTree);
}

void AAIGeneralController::ontargetperception_update_sight(AActor* actor, FAIStimulus const stimulus) {
	if(auto const ch = Cast<ASoldier>(actor)){
		if(GEngine)GEngine->AddOnScreenDebugMessage(960, 1.f, FColor::Green, TEXT("I see: OnTargetPerceptionUpdated"));
	}

	UBlackboardComponent* BlackboardComponent = BrainComponent->GetBlackboardComponent();
	if (BlackboardComponent->GetValueAsObject("ActorSeen0") == nullptr) 
		BlackboardComponent->SetValueAsObject("ActorSeen0", actor);
	else if (BlackboardComponent->GetValueAsObject("ActorSeen1") == nullptr && BlackboardComponent->GetValueAsObject("ActorSeen0") != actor)
		BlackboardComponent->SetValueAsObject("ActorSeen1", actor);
	else if (BlackboardComponent->GetValueAsObject("ActorSeen2") == nullptr && BlackboardComponent->GetValueAsObject("ActorSeen0") != actor && BlackboardComponent->GetValueAsObject("ActorSeen1") != actor)
		BlackboardComponent->SetValueAsObject("ActorSeen2", actor);
};

void AAIGeneralController::onperception_update_sight(const TArray<AActor*>& AArray) {
	if (GEngine)GEngine->AddOnScreenDebugMessage(959, 1.f, FColor::Green, TEXT("I see: OnPerceptionUpdated"));

	//todo clear Focus when Soldier out of range

	UBlackboardComponent* BlackboardComponent = BrainComponent->GetBlackboardComponent();
	if (AArray.Num() > 0 && BlackboardComponent->GetValueAsObject("ActorSeen0") != nullptr) this->SetFocus(Cast<AActor>(BlackboardComponent->GetValueAsObject("ActorSeen0")));
	else this->ClearFocus(EAIFocusPriority::Gameplay);

	


	
};

void AAIGeneralController::setup_perception_system() {
	// create and initialise sight configuration object
	sight_config = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (sight_config)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		sight_config->SightRadius = 1000.0f;
		sight_config->LoseSightRadius = sight_config->SightRadius + 200.0f;
		sight_config->PeripheralVisionAngleDegrees = 82.5f;
		sight_config->SetMaxAge(.2f);
		sight_config->AutoSuccessRangeFromLastSeenLocation = 100.0f;
		sight_config->DetectionByAffiliation.bDetectEnemies = true;
		sight_config->DetectionByAffiliation.bDetectFriendlies = true;
		sight_config->DetectionByAffiliation.bDetectNeutrals = true;

		// add sight configuration component to perception component
		GetPerceptionComponent()->SetDominantSense(*sight_config->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAIGeneralController::ontargetperception_update_sight);
		GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AAIGeneralController::onperception_update_sight);
		GetPerceptionComponent()->ConfigureSense(*sight_config);
	}
};

void AAIGeneralController::setup_BehaviorTree() {
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/BT_AITest.BT_AITest'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}

EPathFollowingRequestResult::Type AAIGeneralController::MoveToActorLocation() {
	UBlackboardComponent* BlackboardComponent = BrainComponent->GetBlackboardComponent();
	AActor* _actor = Cast<AActor>(BlackboardComponent->GetValueAsObject("ActorLocation"));

	EPathFollowingRequestResult::Type _movetoResult = MoveToActor(_actor);

	return _movetoResult;
}

EPathFollowingRequestResult::Type AAIGeneralController::MoveToVectorLocation() {
	UBlackboardComponent* BlackboardComponent = BrainComponent->GetBlackboardComponent();
	FVector _vector = BlackboardComponent->GetValueAsVector("VectorLocation");

	ASoldierAI* _soldier = Cast<ASoldierAI>(GetPawn());

	_soldier->GetCharacterMovement()->MaxWalkSpeed = 2200.f;

	GEngine->AddOnScreenDebugMessage(10, 1.f, FColor::Red, TEXT("Je dois bouger !!!"));
	EPathFollowingRequestResult::Type _movetoResult = MoveToLocation(_vector);

	return _movetoResult;
}

void AAIGeneralController::ShootEnemy() {
};