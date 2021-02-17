// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToLocationBTTaskNode.h"
#include "../../AI/AIGeneralController.h"

UMoveToLocationBTTaskNode::UMoveToLocationBTTaskNode() {
	NodeName = "MoveToVectorLocation";
	bNotifyTick = true;
}

EBTNodeResult::Type UMoveToLocationBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	EPathFollowingRequestResult::Type MoveToActorResult = _controller->MoveToVectorLocation();

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal)
		NodeResult = EBTNodeResult::Succeeded;
	return NodeResult;
}

void UMoveToLocationBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	EPathFollowingRequestResult::Type MoveToActorResult = _controller->MoveToVectorLocation();

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	if(MoveToActorResult == EPathFollowingRequestResult::Failed)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

FString UMoveToLocationBTTaskNode::GetStaticDescription() const {
	return TEXT("Move the AI to a Location (Vector type)");
}
