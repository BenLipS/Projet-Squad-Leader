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

	ResultState _result = AIGeneralController->LaunchGrenade();
	if (_result == ResultState::Success)
		return EBTNodeResult::Succeeded;

	return EBTNodeResult::Failed;
}

/** Permet de d�finir une description pour la t�che. C'est ce texte qui
 appara�tra dans le noeud que nous ajouterons au Behavior Tree */
FString ULauchGrenadeBTTaskNode::GetStaticDescription() const
{
	return TEXT("Launch a grenade");
}


