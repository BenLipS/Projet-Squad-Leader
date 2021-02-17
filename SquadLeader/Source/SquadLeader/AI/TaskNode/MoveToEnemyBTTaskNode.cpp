// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToEnemyBTTaskNode.h"
#include "../../AI/AIGeneralController.h"

UMoveToEnemyBTTaskNode::UMoveToEnemyBTTaskNode() {
	NodeName = "MoveToEnemyLocation";
	bNotifyTick = true;
}

EBTNodeResult::Type UMoveToEnemyBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	EPathFollowingRequestResult::Type MoveToActorResult = _controller->MoveToEnemyLocation();

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal)
		NodeResult = EBTNodeResult::Succeeded;
	return NodeResult;
}

void UMoveToEnemyBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	EPathFollowingRequestResult::Type MoveToActorResult = _controller->MoveToEnemyLocation();

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

FString UMoveToEnemyBTTaskNode::GetStaticDescription() const {
	return TEXT("Move the AI to The Enemy Location");
}
