// Fill out your copyright notice in the Description page of Project Settings.


#include "HeavyAIBasicController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MineTargetPoint.h"
#include "EngineUtils.h"
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
	//Super::Tick(DeltaSeconds);
}

void AHeavyAIBasicController::HomeTick(float DeltaSeconds) {
	Super::HomeTick(DeltaSeconds);

	CheckIfRegenShield();

	//Check if launch Mine
	if (Blackboard->GetValueAsBool("is_patroling") && !Blackboard->GetValueAsBool("LaunchMine") && !Cast<ASoldier>(GetPawn())->IsInCooldown(FGameplayTag::RequestGameplayTag(FName("Cooldown.LaunchProjectile.Mine")))) {
		TArray<AMineTargetPoint*> ListMinePos;
		for (TActorIterator<AMineTargetPoint> It(GetWorld()); It; ++It) {
			AMineTargetPoint* MinePose = *It;
			if ((MinePose->GetActorLocation() - Cast<ASoldier>(GetPawn())->GetActorLocation()).Size() < HalfRadiusPatrol)
				ListMinePos.Add(MinePose);
		}
		if (ListMinePos.Num() > 0) {
			int index = FMath::RandRange(0, ListMinePos.Num() - 1);
			Blackboard->SetValueAsVector("MinePosition", ListMinePos[index]->GetActorLocation());
			Blackboard->SetValueAsBool("LaunchMine", true);
			//StopCurrentBehavior = true;
		}
	}
}

void AHeavyAIBasicController::setup_BehaviorTree()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/AIBasic/BTForClasses/BT_AIBasic_Heavy.BT_AIBasic_Heavy'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}