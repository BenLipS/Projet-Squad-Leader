#include "SquadLeaderGameState.h"
#include "SquadLeader/ControlArea/ControlAreaManager.h"
#include "SquadLeader/UI/Interface/MinimapInterface.h"
#include "SquadLeader/UI/HUD/SL_HUD.h"

ASquadLeaderGameState::ASquadLeaderGameState()
{
	bReplicates = true;
}

void ASquadLeaderGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASquadLeaderGameState, ControlAreaManager);
	DOREPLIFETIME(ASquadLeaderGameState, SoldierTeamCollection);
}

void ASquadLeaderGameState::AddSoldierTeam(ASoldierTeam* _SoldierTeam)
{
	SoldierTeamCollection.AddUnique(_SoldierTeam);
	OnSoldierTeamAddedToList.Broadcast(_SoldierTeam);
}

TArray<ASoldierTeam*> ASquadLeaderGameState::GetSoldierTeamCollection()
{
	return SoldierTeamCollection;
}

ASoldierTeam* ASquadLeaderGameState::GetSoldierTeamByID(int TeamId)
{
	for (ASoldierTeam* team: SoldierTeamCollection) {
		if (team->Id == TeamId)
			return team;
	}
	return nullptr;
}

void ASquadLeaderGameState::SetControlAreaManager(AControlAreaManager* _ControlAreaManager)
{
	ControlAreaManager = _ControlAreaManager;
}

AControlAreaManager* ASquadLeaderGameState::GetControlAreaManager()
{
	// Spawn a controleAreaManager if doesn't exist - this will be used for clients who didn't get the manager replicated
	if (!ControlAreaManager)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
		ControlAreaManager = GetWorld()->SpawnActor<AControlAreaManager>(SpawnInfo);

		for (auto SceneActors : GetWorld()->PersistentLevel->Actors)  // cycle each actor
		{
			if (AControlArea* ControlArea = Cast<AControlArea>(SceneActors); ControlArea)
				ControlAreaManager->AddControlArea(ControlArea);
		}
	}
	return ControlAreaManager;
}
