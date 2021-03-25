// Fill out your copyright notice in the Description page of Project Settings.


#include "../../AI/AIGeneralController.h"
#include "CapturingBTTaskNode.h"

UCapturingBTTaskNode::UCapturingBTTaskNode() {
	NodeName = "Capturing a ControlArea";
	bNotifyTick = true;
}

EBTNodeResult::Type UCapturingBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* m_ai = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	ResultState result = m_ai->Capturing();
	if (result == ResultState::Success)
		return EBTNodeResult::Succeeded;
	if(result == ResultState::InProgress)
		return EBTNodeResult::Failed;
	return EBTNodeResult::InProgress;
}

void UCapturingBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIGeneralController* m_ai = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	ResultState result = m_ai->Capturing();
	if (result == ResultState::Success)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	if (result == ResultState::InProgress)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
}

FString UCapturingBTTaskNode::GetStaticDescription() const {
	return TEXT("Capture a Control Area");
}