// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToLocationBTTaskNode.h"
#include "../../AI/AIGeneralController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"

UMoveToLocationBTTaskNode::UMoveToLocationBTTaskNode() {
	NodeName = "MoveToVectorLocation";
	bNotifyTick = true;
}

EBTNodeResult::Type UMoveToLocationBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	return NodeResult;
}

void UMoveToLocationBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	const FVector LocationGoal = _controller->get_blackboard()->GetValueAsVector(BlackboardKey.SelectedKeyName);

	EPathFollowingRequestResult::Type MoveToActorResult = _controller->MoveToLocation(LocationGoal, AcceptableRadius, true, true, true);

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	if(MoveToActorResult == EPathFollowingRequestResult::Failed || _controller->StopCurrentBehavior)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

FString UMoveToLocationBTTaskNode::GetStaticDescription() const {
	return TEXT("Move the AI to a Location (Vector type)");
}
