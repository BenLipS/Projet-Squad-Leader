// Fill out your copyright notice in the Description page of Project Settings.


#include "LaunchMineBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "../../AI/AIGeneralController.h"

ULaunchMineBTTaskNode::ULaunchMineBTTaskNode() {
	NodeName = "Launch Mine";
}

EBTNodeResult::Type ULaunchMineBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("%s Launch Mine"), *AIGeneralController->GetName()));

	ResultState _result = AIGeneralController->LaunchMine();

	AIGeneralController->get_blackboard()->SetValueAsBool("LaunchMine", false);

	if (_result == ResultState::Success)
		return EBTNodeResult::Succeeded;

	return EBTNodeResult::Failed;
}

/** Permet de définir une description pour la tâche. C'est ce texte qui
 apparaîtra dans le noeud que nous ajouterons au Behavior Tree */
FString ULaunchMineBTTaskNode::GetStaticDescription() const
{
	return TEXT("Launch Mine");
}

