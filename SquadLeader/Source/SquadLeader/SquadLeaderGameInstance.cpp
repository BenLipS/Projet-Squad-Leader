// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadLeaderGameInstance.h"

void USquadLeaderGameInstance::InitAIManagers()
{
	if(GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Game Instance InitAIManagers()"));

	/*Init AIBasic Manager*/

	/*Init AISquad Manager*/

}

void USquadLeaderGameInstance::AddAIBasicToManager(AAIBasicController* AIBasic)
{
	/*if (Cast<ASoldier>(AIBasic->GetPawn())->PlayerTeam == 1) {
		AIBasicManagerTeam1->AIBasicList.Add(AIBasic);
	}
	else if (Cast<ASoldier>(AIBasic->GetPawn())->PlayerTeam == 2) {
		AIBasicManagerTeam2->AIBasicList.Add(AIBasic);
	}
	else {
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Une AI n'a pas d'equipe"));
	}*/
}
