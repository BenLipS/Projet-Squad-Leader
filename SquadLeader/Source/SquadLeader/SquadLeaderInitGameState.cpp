// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadLeaderInitGameState.h"
#include "SquadLeaderGameModeBase.h"

ASquadLeaderInitGameState::ASquadLeaderInitGameState()
{
}

// initialization of variables
void ASquadLeaderInitGameState::InitMapAndData()
{
	// server only
	if (GetLocalRole() == ROLE_Authority) {
		ASquadLeaderGameModeBase* SLGameMode = Cast<ASquadLeaderGameModeBase>(GameModeClass.GetDefaultObject());

		// Clean Managers and collections
		for (auto team : SLGameMode->SoldierTeamCollection) {  // clean all team data at the begining
			team.GetDefaultObject()->CleanSpawnPoints();
			team.GetDefaultObject()->CleanSoldierList();
		}

		SLGameMode->ControlAreaManager.GetDefaultObject()->CleanControlAreaList();  // clean the list of all control area

		//Init static world actors
		InitActorInWorld();
	}

	// for both side (client and server)

}

void ASquadLeaderInitGameState::InitActorInWorld()
{
	for (auto SceneActors : GetWorld()->PersistentLevel->Actors)  // cycle each actor
	{
		if (auto PreInitialisableObject = Cast<IPreInitable>(SceneActors); PreInitialisableObject)
			PreInitialisableObject->PreInitialisation();
	}
}
