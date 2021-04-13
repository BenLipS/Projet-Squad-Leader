// Fill out your copyright notice in the Description page of Project Settings.


#include "CapturingBTTaskNode.h"
#include "../../AI/AIGeneralController.h"

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
	else if (result == ResultState::Failed || Cast<AAIGeneralController>(OwnerComp.GetOwner())->StopCurrentBehavior)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	else
		FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
}

FString UCapturingBTTaskNode::GetStaticDescription() const {
	return TEXT("Capture a Control Area");
}