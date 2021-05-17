#include "SL_HUBPlayerState.h"
#include "SL_HUBGameStateBase.h"
#include "../SquadLeaderGameInstance.h"

void ASL_HUBPlayerState::BeginPlay()
{ // begin play for each player wanting to join a new game
	// create HUBPlayer Param and give it information if available
	LocalHUBPlayerParam = NewObject<AHUBPlayerParam>();
	if (GetPawn()->IsLocallyControlled()) {
		auto GI = GetGameInstance<USquadLeaderGameInstance>();
		LocalHUBPlayerParam->SetPlayerId(GI->GetPlayerId());
		LocalHUBPlayerParam->SetPlayerName(GI->GetPlayerName());
		LocalHUBPlayerParam->SetIsReady(false);
		LocalHUBPlayerParam->SetChoosenTeam(1);
		GI->PlayerParam.GetDefaultObject()->SetTeam(1);

		// send it to HUB GameState
		if (auto GS = GetWorld()->GetGameState<ASL_HUBGameStateBase>(); GS) {
			GS->MulticastSetNewArrival(LocalHUBPlayerParam);
		}
	}
}

void ASL_HUBPlayerState::ChangeTeam()
{
	int NewTeamId = (LocalHUBPlayerParam->GetChoosenTeam() % 2) + 1;
	if (GetPawn()->IsLocallyControlled()) {  // send data to the LocalPlayerParam
		LocalHUBPlayerParam->SetChoosenTeam(NewTeamId);
		if (auto GS = GetWorld()->GetGameState<ASL_HUBGameStateBase>(); GS)
			GS->MulticastUpdatePlayer(LocalHUBPlayerParam);

		auto GI = GetGameInstance<USquadLeaderGameInstance>();
		GI->PlayerParam.GetDefaultObject()->SetTeam(NewTeamId);
	}
}

void ASL_HUBPlayerState::ChangeReadyState()
{
	bool NewReadyState = !LocalHUBPlayerParam->GetIsReady();
	if (GetPawn()->IsLocallyControlled()) {  // send data to the LocalPlayerParam
		LocalHUBPlayerParam->SetChoosenTeam(NewReadyState);
		if (auto GS = GetWorld()->GetGameState<ASL_HUBGameStateBase>(); GS)
			GS->MulticastUpdatePlayer(LocalHUBPlayerParam);
	}
}

void ASL_HUBPlayerState::ClientRemoveHUBPlayerParam_Implementation()
{
	if (auto GS = GetWorld()->GetGameState<ASL_HUBGameStateBase>(); GS)
		GS->MulticastRemovePlayer(LocalHUBPlayerParam->GetPlayerID());
}
