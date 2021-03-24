// Fill out your copyright notice in the Description page of Project Settings.


#include "EndSearchBTTaskNode.h"
#include "../../AI/AIGeneralController.h"

UEndSearchBTTaskNode::UEndSearchBTTaskNode() {
	NodeName = "End the research of the enemy";
	bNotifyTick = false;
}

EBTNodeResult::Type UEndSearchBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	_controller->EndTheResearch();

	return EBTNodeResult::Succeeded;
}

FString UEndSearchBTTaskNode::GetStaticDescription() const {
	return TEXT("The AI stop searching for the enemy after 2seconds");

}