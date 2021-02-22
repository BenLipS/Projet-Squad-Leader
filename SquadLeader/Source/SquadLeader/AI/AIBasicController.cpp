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


AAIBasicController::AAIBasicController() :
	AlignmentWeight{ 1.f },
	CohesionWeight{ 1.f },
	SeparationWeight{ 1.f },
	ObjectifWeight{ 1.f },
	SeparationVector{ 0.f, 0.f, 0.f },
	AlignementVector{ 0.f, 0.f, 0.f },
	CohesionVector{ 0.f, 0.f, 0.f },
	ObjectifVector{ 0.f, 0.f, 0.f },
	ObjectifLocation{ 1000.f, 0.f, 10.f },
	MovementVector{ 0.f, 0.f, 0.f },
	MovementVectorScale{2.f}
{
	setup_BehaviorTree();
}


void AAIBasicController::BeginPlay() {
	Super::BeginPlay();
}

void AAIBasicController::setup_BehaviorTree() {
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/BT_AIBasic.BT_AIBasic'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}

void AAIBasicController::Tick(float DeltaSeconds) {
	FVector SoldierLocation = GetPawn()->GetActorLocation();
	ObjectifVector = ObjectifLocation - SoldierLocation;
	AlignementVector.Normalize();
	CohesionVector.Normalize();
	SeparationVector.Normalize();
	ObjectifVector.Normalize();
	MovementVector = AlignementVector * AlignmentWeight 
		+ CohesionVector * CohesionWeight
		+ SeparationVector * SeparationWeight
		+ ObjectifVector * ObjectifWeight;
	float MaxSpeed = GetPawn()->GetMovementComponent()->GetMaxSpeed();
	MovementVector = (MovementVector * 100 * DeltaSeconds).GetClampedToMaxSize(MaxSpeed * DeltaSeconds);
	blackboard->SetValueAsVector("FlockingLocation" , SoldierLocation + MovementVector);
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + ObjectifVector, FColor::Red);
	DrawDebugPoint(GetWorld(), ObjectifLocation, 12, FColor::Green);
}
