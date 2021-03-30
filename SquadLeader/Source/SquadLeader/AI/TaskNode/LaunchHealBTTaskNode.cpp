// Fill out your copyright notice in the Description page of Project Settings.


#include "LaunchHealBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "../../AI/AIGeneralController.h"

ULaunchHealBTTaskNode::ULaunchHealBTTaskNode() {
	NodeName = "Launch Heal";
}

EBTNodeResult::Type ULaunchHealBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	ResultState _result = AIGeneralController->LaunchHeal();

	AIGeneralController->get_blackboard()->SetValueAsBool("LaunchHeal", false);

	if (_result == ResultState::Success)
		return EBTNodeResult::Succeeded;

	return EBTNodeResult::Failed;
}

/** Permet de définir une description pour la tâche. C'est ce texte qui
 apparaîtra dans le noeud que nous ajouterons au Behavior Tree */
FString ULaunchHealBTTaskNode::GetStaticDescription() const
{
	return TEXT("Launch Heal");
}


