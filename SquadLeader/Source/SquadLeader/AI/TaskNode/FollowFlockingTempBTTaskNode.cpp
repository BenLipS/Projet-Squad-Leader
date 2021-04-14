// Fill out your copyright notice in the Description page of Project Settings.


#include "FollowFlockingTempBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../AIBasicController.h"
#include "../AISquadController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UFollowFlockingTempBTTaskNode::UFollowFlockingTempBTTaskNode()
{
	NodeName = "FollowFlocking";
	bNotifyTick = true;
}

EBTNodeResult::Type UFollowFlockingTempBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	// Obtenir un pointeur sur AIEnemyController
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	FBlackboard::FKey tedst = BlackboardKey.GetSelectedKeyID();

	const FVector _TempObjectif = AIGeneralController->get_blackboard()->GetValueAsVector(BlackboardKey.SelectedKeyName);
	
	AIGeneralController->SetTempObjectifLocation(AIGeneralController->GetRealObjectifLocation());
	AIGeneralController->SetObjectifLocation(_TempObjectif);

	AIGeneralController->FollowFlocking();
	//Nous retournons Succeeded
	return EBTNodeResult::InProgress;
}

void UFollowFlockingTempBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	EPathFollowingRequestResult::Type MoveToActorResult = AIGeneralController->FollowFlocking();

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal) {
		AIGeneralController->SetObjectifLocation(AIGeneralController->GetTempObjectifLocation());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else if (MoveToActorResult == EPathFollowingRequestResult::Failed) {
		AIGeneralController->SetObjectifLocation(AIGeneralController->GetTempObjectifLocation());
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	if (AAISquadController* AISquadController = Cast<AAISquadController>(OwnerComp.GetOwner()); AISquadController && AISquadController->get_blackboard()->GetValueAsBool("IsInFormation") == true) {
		AIGeneralController->SetObjectifLocation(AIGeneralController->GetTempObjectifLocation());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

FString UFollowFlockingTempBTTaskNode::GetStaticDescription() const
{
	return TEXT("Follow the Flock");
}
