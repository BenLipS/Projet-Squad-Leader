// Fill out your copyright notice in the Description page of Project Settings.


#include "HeavyAISquadController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "BehaviorTree/BehaviorTree.h"
#include "../../Soldiers/Soldier.h"

AHeavyAISquadController::AHeavyAISquadController() {
	setup_BehaviorTree();
}

void AHeavyAISquadController::CheckIfRegenShield() {
	if (SeenEnemySoldier.Num() > 2 && !Cast<ASoldier>(GetPawn())->IsInCooldown(FGameplayTag::RequestGameplayTag(FName("Cooldown.AreaEffectFromSelf.Temporary.MaxShield")))) {
		Blackboard->SetValueAsBool("RegenShield", true);
		StopCurrentBehavior = true;
	}
}

void AHeavyAISquadController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	CheckIfRegenShield();

}

void AHeavyAISquadController::setup_BehaviorTree()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/BT_Squad_Classes/BT_AISquad_Heavy.BT_AISquad_Heavy'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}