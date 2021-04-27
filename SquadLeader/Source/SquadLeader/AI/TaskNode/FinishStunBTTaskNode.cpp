// Fill out your copyright notice in the Description page of Project Settings.


#include "FinishStunBTTaskNode.h"
#include "../../AI/AIGeneralController.h"
#include "BehaviorTree/BlackboardComponent.h"

UFinishStunBTTaskNode::UFinishStunBTTaskNode() {
	NodeName = "End Stun";
	bNotifyTick = false;
}

EBTNodeResult::Type UFinishStunBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	_controller->get_blackboard()->SetValueAsBool("IsStun", false);

	return EBTNodeResult::Succeeded;
}

FString UFinishStunBTTaskNode::GetStaticDescription() const {
	return TEXT("End Stun");

}
