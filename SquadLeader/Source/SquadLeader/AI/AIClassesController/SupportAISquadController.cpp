// Fill out your copyright notice in the Description page of Project Settings.


#include "SupportAISquadController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "BehaviorTree/BehaviorTree.h"
#include "../AISquadManager.h"
#include "../../Soldiers/Soldier.h"

ASupportAISquadController::ASupportAISquadController() {
	setup_BehaviorTree();
}

void ASupportAISquadController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	//TODO Make a function with that LaunchHeal()
	int TotalHealth = 0;
	int TotalMaxHealth = 0;
	float RatioHealth = 0;

	bool IsAnAllyHealthVeryLow = false;

	if (SquadManager) {
		for (auto AISquad : SquadManager->AISquadList) {
			TotalHealth += Cast<ASoldier>(AISquad->GetPawn())->GetHealth();
			TotalMaxHealth += Cast<ASoldier>(AISquad->GetPawn())->GetMaxHealth();
			if (Cast<ASoldier>(AISquad->GetPawn())->GetHealth() / Cast<ASoldier>(AISquad->GetPawn())->GetMaxHealth() < SinglePlayerRatioBeforeHeal)
				IsAnAllyHealthVeryLow = true;
		}

		if(SquadManager->Leader->GetHealth() / SquadManager->Leader->GetMaxHealth() < SinglePlayerRatioBeforeHeal)
			IsAnAllyHealthVeryLow = true;

		RatioHealth = TotalHealth / TotalMaxHealth;

		if ( (RatioHealth < RatioBeforeHeal || IsAnAllyHealthVeryLow ) && !SquadManager->IsASquadMemberHealing && !Cast<ASoldier>(GetPawn())->IsInCooldown(FGameplayTag::RequestGameplayTag(FName("Cooldown.AreaEffectFromSelf.Instant.Heal")))) {
			Blackboard->SetValueAsBool("LaunchHeal", true);
			SquadManager->IsASquadMemberHealing = true;
			StopCurrentBehavior = true;
		}
	}

}

void ASupportAISquadController::setup_BehaviorTree()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/BT_Squad_Classes/BT_AISquad_Support.BT_AISquad_Support'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}