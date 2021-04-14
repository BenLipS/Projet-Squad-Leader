#include "SquadLeaderInitGameState.h"
#include "../SquadLeaderGameModeBase.h"
#include "../Interface/PreInitable.h"
#include "../Soldiers/SoldierTeam.h"
#include "../ControlArea/ControlAreaManager.h"

ASquadLeaderInitGameState::ASquadLeaderInitGameState()
{
}

// initialization of variables
void ASquadLeaderInitGameState::InitMapAndData()
{
	// server only
	if (GetLocalRole() == ROLE_Authority) {
		ASquadLeaderGameModeBase* SLGameMode = Cast<ASquadLeaderGameModeBase>(GameModeClass.GetDefaultObject());
		//Init static world actors
		InitActorInWorld();

		// Clean Managers and collections
		for (auto team : SoldierTeamCollection) {  // clean all team data at the begining
			team->CleanSpawnPoints();
			team->CleanSoldierList();
		}

		ControlAreaManager->CleanControlAreaList();  // clean the list of all control area

	}

	// for both side (client and server)

}

void ASquadLeaderInitGameState::InitActorInWorld()
{
	PlaceActorInWorld();

	TArray<IPreInitable*> InitList;
	for (auto SceneActors : GetWorld()->PersistentLevel->Actors)  // cycle each actor
	{
		if (auto PreInitialisableObject = Cast<IPreInitable>(SceneActors); PreInitialisableObject) {
			InitList.Add(PreInitialisableObject);  // add actor to list if preInitialisable
		}
	}

	//sort the list using priority
	Algo::Sort(InitList, [](IPreInitable* a, IPreInitable* b) {
		return a->GetPriority() < b->GetPriority();
		});

	for (IPreInitable* Element:InitList) {
		Element->PreInitialisation();
	}
	ensure(SoldierTeamCollection.Num() >= 2);  // check if TeamCollection have at least 2 elements
	ensure(ControlAreaManager->GetControlArea().Num() >= 1);  // check if ControlAreaManager exist and have at least one ControlArea
}

void ASquadLeaderInitGameState::PlaceActorInWorld()
{
	// Place ControlAreaManager in world

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	AControlAreaManager* CAM = GetWorld()->SpawnActor<AControlAreaManager>(SpawnInfo);

	// more spawn if needed
}