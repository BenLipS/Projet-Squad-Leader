// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeatBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "../../AI/AIGeneralController.h"
#include "../../AI/AISquadController.h"
#include "../../AI/AISquadManager.h"

UOverHeatBTTaskNode::UOverHeatBTTaskNode() {
	NodeName = "OverHeat";
}

EBTNodeResult::Type UOverHeatBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("%s OverHeat"), *AIGeneralController->GetName()));
	ResultState _result = AIGeneralController->OverHeat();

	AIGeneralController->get_blackboard()->SetValueAsBool("OverHeat", false);

	if (_result == ResultState::Success)
		return EBTNodeResult::Succeeded;

	return EBTNodeResult::Failed;
}

/** Permet de définir une description pour la tâche. C'est ce texte qui
 apparaîtra dans le noeud que nous ajouterons au Behavior Tree */
FString UOverHeatBTTaskNode::GetStaticDescription() const
{
	return TEXT("OverHeat");
}

