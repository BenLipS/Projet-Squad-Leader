// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootEnemyBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../AI/AIGeneralController.h"

UShootEnemyBTTaskNode::UShootEnemyBTTaskNode() {
	NodeName = "Shoot Enemy Task";
	bNotifyTick = true;
}

EBTNodeResult::Type UShootEnemyBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	ResultState _result = AIGeneralController->ShootEnemy();
	if (_result == ResultState::Success)
		return EBTNodeResult::Succeeded;
	if (_result == ResultState::Failed)
		return EBTNodeResult::Failed;
	return EBTNodeResult::InProgress;
}

void UShootEnemyBTTaskNode::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIGeneralController* _controller = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	ResultState _result = _controller->ShootEnemy();
	if (_result == ResultState::Success) {
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		_controller->get_blackboard()->SetValueAsVector("EnemyLocation", FVector());
		_controller->get_blackboard()->SetValueAsBool("is_attacking", false);
		_controller->get_blackboard()->SetValueAsObject("FocusActor", NULL);
	}
	else if (_result == ResultState::Failed)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

/** Permet de définir une description pour la tâche. C'est ce texte qui
 apparaîtra dans le noeud que nous ajouterons au Behavior Tree */
FString UShootEnemyBTTaskNode::GetStaticDescription() const
{
	return TEXT("Shoot on an enemy until he is dead");
}
