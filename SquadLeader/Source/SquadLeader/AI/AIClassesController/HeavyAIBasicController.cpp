// Fill out your copyright notice in the Description page of Project Settings.


#include "HeavyAIBasicController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "BehaviorTree/BehaviorTree.h"
#include "../../Soldiers/Soldier.h"

AHeavyAIBasicController::AHeavyAIBasicController() {
	setup_BehaviorTree();
}

void AHeavyAIBasicController::CheckIfRegenShield() {
	if (SeenEnemySoldier.Num() > 2  && !Cast<ASoldier>(GetPawn())->IsInCooldown(FGameplayTag::RequestGameplayTag(FName("Cooldown.AreaEffectFromSelf.Temporary.MaxShield")))) {
		Blackboard->SetValueAsBool("RegenShield", true);
		StopCurrentBehavior = true;
	}
}

void AHeavyAIBasicController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	CheckIfRegenShield();

}

void AHeavyAIBasicController::setup_BehaviorTree()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/AIBasic/BTForClasses/BT_AIBasic_Heavy.BT_AIBasic_Heavy'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}