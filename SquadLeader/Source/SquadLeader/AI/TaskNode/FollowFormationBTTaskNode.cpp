// Fill out your copyright notice in the Description page of Project Settings.


#include "FollowFormationBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../AISquadController.h"

UFollowFormationBTTaskNode::UFollowFormationBTTaskNode()
{
	NodeName = "FollowFormation";
	bNotifyTick = true;
}

EBTNodeResult::Type UFollowFormationBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	// Obtenir un pointeur sur AIEnemyController
	AAISquadController* AISquadController = Cast<AAISquadController>(OwnerComp.GetOwner());
	// Appeler la fonctionUpdateNextTargetPoint qui contient la logique pour sélectionner
	 // le prochain TargetPoint
	AISquadController->FollowFormation();
	//Nous retournons Succeeded
	return EBTNodeResult::InProgress;
}

void UFollowFormationBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAISquadController* AISquadController = Cast<AAISquadController>(OwnerComp.GetOwner());

	EPathFollowingRequestResult::Type MoveToActorResult = AISquadController->FollowFormation();

	if (MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal && !AISquadController->get_blackboard()->GetValueAsBool("IsInFormation") || AISquadController->get_blackboard()->GetValueAsBool("HasOrder")) {
		Cast<ASoldierAI>(AISquadController->GetPawn())->CancelAbilityRun();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
		if (MoveToActorResult == EPathFollowingRequestResult::Failed) {
			Cast<ASoldierAI>(AISquadController->GetPawn())->CancelAbilityRun();
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

}

FString UFollowFormationBTTaskNode::GetStaticDescription() const
{
	return TEXT("Follow the Flock");
}

