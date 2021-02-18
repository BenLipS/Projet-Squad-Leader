// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootEnemy.h"
#include "AIGeneralController.h"

/** Constructeur de la classe */
UShootEnemy::UShootEnemy()
{
	// Nous définissons le nom que protera le noeud dans le BT
	NodeName = "ShootEnemy";
}
/* Fonction d'exécution de la tâche, cette tâche devra retourner Succeeded, Failed ou InProgress */
EBTNodeResult::Type UShootEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{

	AAIGeneralController* AIGeneralController = Cast<AAIGeneralController>(OwnerComp.GetOwner());

	AIGeneralController->ShootEnemy();
	//Nous retournons Succeeded
	return EBTNodeResult::Succeeded;
}
/** Permet de définir une description pour la tâche. C'est ce texte qui
 apparaîtra dans le noeud que nous ajouterons au Behavior Tree */
FString UShootEnemy::GetStaticDescription() const
{
	return TEXT("Shoot on an enemy");
}

