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
	// Appeler la fonctionUpdateNextTargetPoint qui contient la logique pour slectionner
	 // le prochain TargetPoint
	HysteresisDoFollow = false;
	AISquadController->HysteresisDoRunningFormation = false;
	Cast<ASoldierAI>(AISquadController->GetPawn())->CancelAbilityRun();
	AISquadController->IsRunning = false;
	AISquadController->FollowFormation();
	AISquadController->FollowFlocking();
	//Nous retournons Succeeded
	return EBTNodeResult::InProgress;
}

void UFollowFormationBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAISquadController* AISquadController = Cast<AAISquadController>(OwnerComp.GetOwner());
	if ((AISquadController->FormationPosBeforeTransform - AISquadController->GetPawn()->GetActorLocation()).Size() < AISquadController->StopHysteresisDistanceForFormation)
		HysteresisDoFollow = false;
	if ((AISquadController->FormationPosBeforeTransform - AISquadController->GetPawn()->GetActorLocation()).Size() > AISquadController->HysteresisDistanceForFormation)
		HysteresisDoFollow = true;

	if (HysteresisDoFollow) {
		AISquadController->FollowFormation();
		EPathFollowingRequestResult::Type MoveToActorResult = AISquadController->FollowFlocking();
	}

	if (/*MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal && */!AISquadController->get_blackboard()->GetValueAsBool("IsInFormation") || AISquadController->get_blackboard()->GetValueAsBool("HasOrder") || AISquadController->StopCurrentBehavior || AISquadController->get_blackboard()->GetValueAsBool("LaunchHeal")) {
		Cast<ASoldierAI>(AISquadController->GetPawn())->CancelAbilityRun();
		AISquadController->IsRunning = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if ((AISquadController->FormationPosBeforeTransform - (Cast<AAISquadController>(OwnerComp.GetOwner())->GetPawn()->GetActorLocation())).Size() < AISquadController->StopHysteresisRunningDistanceForFormation)
		AISquadController->HysteresisDoRunningFormation = false;
	if ((AISquadController->FormationPosBeforeTransform - (Cast<AAISquadController>(OwnerComp.GetOwner())->GetPawn()->GetActorLocation())).Size() > AISquadController->HysteresisRunningDistanceForFormation)
		AISquadController->HysteresisDoRunningFormation = true;

	if (AISquadController->HysteresisDoRunningFormation && !AISquadController->IsRunning) {
		Cast<ASoldierAI>(Cast<AAISquadController>(OwnerComp.GetOwner())->GetPawn())->ActivateAbilityRun();
		AISquadController->IsRunning = true;
	}
	else if (!AISquadController->HysteresisDoRunningFormation && AISquadController->IsRunning) {
		Cast<ASoldierAI>(Cast<AAISquadController>(OwnerComp.GetOwner())->GetPawn())->CancelAbilityRun();
		AISquadController->IsRunning = false;
	}

	//if (MoveToActorResult == EPathFollowingRequestResult::Failed) {
	//	Cast<ASoldierAI>(AISquadController->GetPawn())->CancelAbilityRun();
	//	AISquadController->RunToFormation = false;
	//	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	//}

}

FString UFollowFormationBTTaskNode::GetStaticDescription() const
{
	return TEXT("Follow the Flock");
}

