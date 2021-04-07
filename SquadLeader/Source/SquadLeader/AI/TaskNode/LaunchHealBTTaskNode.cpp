// Fill out your copyright notice in the Description page of Project Settings.


#include "LaunchHealBTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "../../AI/AIGeneralController.h"
#include "../../AI/AISquadController.h"
#include "../../AI/AISquadManager.h"

ULaunchHealBTTaskNode::ULaunchHealBTTaskNode() {
	NodeName = "Launch Heal";
}

EBTNodeResult::Type ULaunchHealBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("%s heal"), *AIGeneralController->GetName()));
	ResultState _result = AIGeneralController->LaunchHeal();

	AIGeneralController->get_blackboard()->SetValueAsBool("LaunchHeal", false);

	if (AAISquadController* AISquadController = Cast<AAISquadController>(OwnerComp.GetOwner()); AISquadController)
		AISquadController->SquadManager->IsASquadMemberHealing = false;

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


