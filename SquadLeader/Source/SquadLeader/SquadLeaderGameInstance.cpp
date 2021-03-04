// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadLeaderGameInstance.h"
#include "SquadLeaderGameModeBase.h"

USquadLeaderGameInstance::USquadLeaderGameInstance() : 
		ListAISquadManagers{}
{
}

void USquadLeaderGameInstance::InitAIManagers()
{
	if(GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Game Instance InitAIManagers()"));
	auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());

	/*Init AIBasic Manager*/
	AIBasicManagerTeam1 = NewObject<UAIBasicManager>();
	AIBasicManagerTeam1->Init(gameMode->SoldierTeamCollection[0]);

	AIBasicManagerTeam2 = NewObject<UAIBasicManager>();
	AIBasicManagerTeam2->Init(gameMode->SoldierTeamCollection[1]);

	/*Init AISquad Manager*/
	//Each Player init a SquadManager in SoldierPlayer.cpp PossessedBy
}

void USquadLeaderGameInstance::AddAIBasicToManager(AAIBasicController* AIBasic)
{
	auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
	if (gameMode && Cast<ASoldier>(AIBasic->GetPawn())->GetTeam() == gameMode->SoldierTeamCollection[0]) {
		AIBasicManagerTeam1->AIBasicList.Add(AIBasic);
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("AIBasic Team 1 added"));
	}
	else if (gameMode && Cast<ASoldier>(AIBasic->GetPawn())->GetTeam() == gameMode->SoldierTeamCollection[1]) {
		AIBasicManagerTeam2->AIBasicList.Add(AIBasic);
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("AIBasic Team 2 added"));
	}
	else {
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Une AI n'a pas d'equipe"));
	}
}
