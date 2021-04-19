// Fill out your copyright notice in the Description page of Project Settings.


#include "IsHitBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "DrawDebugHelpers.h"
#include "../../AI/AIGeneralController.h"

UIsHitBTTaskNode::UIsHitBTTaskNode() {
	NodeName = "Is Hit Task";
	bNotifyTick = true;
}

EBTNodeResult::Type UIsHitBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	_controller->get_blackboard()->SetValueAsBool("IsHit", false);

	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	if(!_controller->IsActivated)
		NodeResult = EBTNodeResult::Failed;

	FVector EnemyPos = _controller->get_blackboard()->GetValueAsVector("IsHitEnemyLocation");
	FVector AIPos = _controller->GetPawn()->GetActorLocation();

	IsHitLocation = AIPos + (EnemyPos - AIPos).GetSafeNormal() * 400.f;

	return NodeResult;
}

void UIsHitBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	EPathFollowingRequestResult::Type MoveToActorResult = _controller->MoveToLocation(IsHitLocation, 50.f, true, true, true);

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	if (MoveToActorResult == EPathFollowingRequestResult::Failed || _controller->StopCurrentBehavior)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

/** Permet de définir une description pour la tâche. C'est ce texte qui
 apparaîtra dans le noeud que nous ajouterons au Behavior Tree */
FString UIsHitBTTaskNode::GetStaticDescription() const
{
	return TEXT("Is Hit");
}


