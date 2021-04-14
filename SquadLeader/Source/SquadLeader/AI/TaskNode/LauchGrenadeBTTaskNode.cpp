// Fill out your copyright notice in the Description page of Project Settings.


#include "LauchGrenadeBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "../../AI/AIGeneralController.h"

ULauchGrenadeBTTaskNode::ULauchGrenadeBTTaskNode() {
	NodeName = "Launch Grenade Enemy Task";
}

EBTNodeResult::Type ULauchGrenadeBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("%s Launch Grenade"), *AIGeneralController->GetName()));

	ResultState _result = AIGeneralController->LaunchGrenade();

	AIGeneralController->get_blackboard()->SetValueAsBool("LaunchGrenade", false);

	if (_result == ResultState::Success)
		return EBTNodeResult::Succeeded;

	return EBTNodeResult::Failed;
}

/** Permet de définir une description pour la tâche. C'est ce texte qui
 apparaîtra dans le noeud que nous ajouterons au Behavior Tree */
FString ULauchGrenadeBTTaskNode::GetStaticDescription() const
{
	return TEXT("Launch a grenade");
}


