// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToSearchEnemyBTTaskNode.h"
#include "../../AI/AIGeneralController.h"

UMoveToSearchEnemyBTTaskNode::UMoveToSearchEnemyBTTaskNode() {
	NodeName = "Search Enemy Location";
	bNotifyTick = true;
}

EBTNodeResult::Type UMoveToSearchEnemyBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory){
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	EPathFollowingRequestResult::Type MoveToActorResult = _controller->MoveToSearchEnemy();

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal)
		NodeResult = EBTNodeResult::Succeeded;
	return NodeResult;
}

void UMoveToSearchEnemyBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	EPathFollowingRequestResult::Type MoveToActorResult = _controller->MoveToSearchEnemy();

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	if (MoveToActorResult == EPathFollowingRequestResult::Failed)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

FString UMoveToSearchEnemyBTTaskNode::GetStaticDescription() const {
	return TEXT("The AI will move to the last known position of the enemy, and search for him. It'll last less than 2seconds to search.");
}