// Fill out your copyright notice in the Description page of Project Settings.


#include "RunToVectorLocationBTTaskNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "../../AI/AIGeneralController.h"
#include "../../Soldiers/AIs/SoldierAI.h"

URunToVectorLocationBTTaskNode::URunToVectorLocationBTTaskNode() {
	NodeName = "RunToVectorLocation";
	bNotifyTick = true;
}

EBTNodeResult::Type URunToVectorLocationBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	Cast<ASoldierAI>(_controller->GetPawn())->ActivateAbilityRun();
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	EPathFollowingRequestResult::Type MoveToActorResult = _controller->MoveToVectorLocation();

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal)
		NodeResult = EBTNodeResult::Succeeded;
	return NodeResult;
}

void URunToVectorLocationBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	const FVector Goal = _controller->get_blackboard()->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());

	EPathFollowingRequestResult::Type MoveToActorResult = _controller->RunToVectorLocation(Goal, AcceptableRadius);

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal)
		Cast<ASoldierAI>(_controller->GetPawn())->CancelAbilityRun();
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	if (MoveToActorResult == EPathFollowingRequestResult::Failed)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

FString URunToVectorLocationBTTaskNode::GetStaticDescription() const {
	return TEXT("RUN the AI to a Location (Vector type)");
}