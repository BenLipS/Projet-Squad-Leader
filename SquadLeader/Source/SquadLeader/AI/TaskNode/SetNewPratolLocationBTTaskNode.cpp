// Fill out your copyright notice in the Description page of Project Settings.

#include "SetNewPratolLocationBTTaskNode.h"
#include "../AIGeneralController.h"

USetNewPratolLocationBTTaskNode::USetNewPratolLocationBTTaskNode()
{
	NodeName = "Set New Patrol Point";
}

EBTNodeResult::Type USetNewPratolLocationBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	// Obtenir un pointeur sur AIEnemyController
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	// Appeler la fonctionUpdateNextTargetPoint qui contient la logique pour sélectionner
	 // le prochain TargetPoint
	AIGeneralController->SetPatrolPoint();
	//Nous retournons Succeeded
	return EBTNodeResult::Succeeded;
}

FString USetNewPratolLocationBTTaskNode::GetStaticDescription() const
{
	return TEXT("Set New Patrol Point");
}

