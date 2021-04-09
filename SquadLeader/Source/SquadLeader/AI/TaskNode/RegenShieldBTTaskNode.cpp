// Fill out your copyright notice in the Description page of Project Settings.


#include "RegenShieldBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "../../AI/AIGeneralController.h"
#include "../../AI/AISquadController.h"
#include "../../AI/AISquadManager.h"

URegenShieldBTTaskNode::URegenShieldBTTaskNode() {
	NodeName = "Regen Shield";
}

EBTNodeResult::Type URegenShieldBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("%s Regen Shield"), *AIGeneralController->GetName()));
	ResultState _result = AIGeneralController->RegenShield();

	AIGeneralController->get_blackboard()->SetValueAsBool("RegenShield", false);

	if (_result == ResultState::Success)
		return EBTNodeResult::Succeeded;

	return EBTNodeResult::Failed;
}

/** Permet de définir une description pour la tâche. C'est ce texte qui
 apparaîtra dans le noeud que nous ajouterons au Behavior Tree */
FString URegenShieldBTTaskNode::GetStaticDescription() const
{
	return TEXT("Regen Shield");
}


