// Fill out your copyright notice in the Description page of Project Settings.


#include "FollowFlockingBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../AIBasicController.h"
#include "../AISquadController.h"

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

	ResultState arrive = AIGeneralController->ArriveAtDestination();

	

	if(arrive == ResultState::Success)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	if(arrive == ResultState::Failed)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	else if (AAISquadController* AISquadController = Cast<AAISquadController>(OwnerComp.GetOwner()); AISquadController && AISquadController->get_blackboard()->GetValueAsBool("IsInFormation") == true) {
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

FString UFollowFlockingBTTaskNode::GetStaticDescription() const
{
	return TEXT("Follow the Flock");
}
