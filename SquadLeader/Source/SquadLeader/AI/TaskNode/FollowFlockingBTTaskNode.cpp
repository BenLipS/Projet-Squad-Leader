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
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	// Appeler la fonctionUpdateNextTargetPoint qui contient la logique pour sélectionner
	 // le prochain TargetPoint
	AIGeneralController->FollowFlocking();
	//Nous retournons Succeeded
	return EBTNodeResult::InProgress;
}

void UFollowFlockingBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	EPathFollowingRequestResult::Type MoveToActorResult = AIGeneralController->FollowFlocking();
	FVector distToObjectif = AIGeneralController->GetPawn()->GetActorLocation() - AIGeneralController->GetObjectifLocation();
	if (distToObjectif.Size() < 500) {
		//AIBasicController->get_blackboard()->SetValueAsBool("DoFlocking", false);
		AIGeneralController->SetState(AIBasicState::Patroling);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else if (AIGeneralController->get_blackboard()->GetValueAsBool("is_attacking")) {
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	/*if (MoveToActorResult == EPathFollowingRequestResult::Failed)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);*/

}

FString UFollowFlockingBTTaskNode::GetStaticDescription() const
{
	return TEXT("Follow the Flock");
}
