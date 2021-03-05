// Fill out your copyright notice in the Description page of Project Settings.


#include "FollowFlockingBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../AIBasicController.h"

UFollowFlockingBTTaskNode::UFollowFlockingBTTaskNode()
{
	NodeName = "FollowFlocking";
	bNotifyTick = true;
}

EBTNodeResult::Type UFollowFlockingBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	// Obtenir un pointeur sur AIEnemyController
	AAIBasicController* AIBasicController = Cast<AAIBasicController>(OwnerComp.GetOwner());
	// Appeler la fonctionUpdateNextTargetPoint qui contient la logique pour sélectionner
	 // le prochain TargetPoint
	AIBasicController->FollowFlocking();
	//Nous retournons Succeeded
	return EBTNodeResult::InProgress;
}

void UFollowFlockingBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIBasicController* AIBasicController = Cast<AAIBasicController>(OwnerComp.GetOwner());

	EPathFollowingRequestResult::Type MoveToActorResult = AIBasicController->FollowFlocking();
	FVector distToObjectif = AIBasicController->GetPawn()->GetActorLocation() - AIBasicController->GetObjectifLocation();
	if (distToObjectif.Size() < 500) {
		//AIBasicController->get_blackboard()->SetValueAsBool("DoFlocking", false);
		AIBasicController->SetState(AIBasicState::Patroling);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	if (MoveToActorResult == EPathFollowingRequestResult::Failed)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

}

FString UFollowFlockingBTTaskNode::GetStaticDescription() const
{
	return TEXT("Follow the Flock");
}
