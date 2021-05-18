#include "SL_HUBPlayerState.h"
#include "SL_HUBGameModeBase.h"
#include "GameFramework/HUD.h"
#include "../UI/Interface/StatInfoInterface.h"
#include "../SquadLeaderGameInstance.h"

void ASL_HUBPlayerState::BeginPlay()
{ // begin play for each player wanting to join a new game
	// create HUBPlayer Param and give it information if available
	LocalHUBPlayerParam = NewObject<AHUBPlayerParam>();

	auto GI = GetGameInstance<USquadLeaderGameInstance>();
	LocalHUBPlayerParam->SetPlayerId(GI->GetPlayerId());
	LocalHUBPlayerParam->SetPlayerName(GI->GetPlayerName());
	LocalHUBPlayerParam->SetIsReady(false);
	LocalHUBPlayerParam->SetChoosenTeam(1);
	GI->PlayerParam.GetDefaultObject()->SetTeam(1);

	ServerSetNewArrival(LocalHUBPlayerParam->GetPlayerID(), LocalHUBPlayerParam->GetPlayerName(), LocalHUBPlayerParam->GetIsReady(), LocalHUBPlayerParam->GetChoosenTeam());

	// set IsInGame state to 2
	auto GI = GetGameInstance<USquadLeaderGameInstance>();
	GI->ChangeNetworkState(2);
}

void ASL_HUBPlayerState::ChangeTeam()
{
	int NewTeamId = (LocalHUBPlayerParam->GetChoosenTeam() % 2) + 1;
	if (auto PC = GetWorld()->GetFirstPlayerController(); PC && PC->GetPlayerState<ASL_HUBPlayerState>() == this) {  // send data to the LocalPlayerParam
		LocalHUBPlayerParam->SetChoosenTeam(NewTeamId);
		ServerUpdatePlayer(LocalHUBPlayerParam->GetPlayerID(), LocalHUBPlayerParam->GetPlayerName(), LocalHUBPlayerParam->GetIsReady(), LocalHUBPlayerParam->GetChoosenTeam());

		auto GI = GetGameInstance<USquadLeaderGameInstance>();
		GI->PlayerParam.GetDefaultObject()->SetTeam(NewTeamId);
	}
}

FString ASL_HUBPlayerState::GetCurrentTeamAsText()
{
	return FString("Team ") + FString::FromInt(LocalHUBPlayerParam->GetChoosenTeam());
}

void ASL_HUBPlayerState::ChangeReadyState(bool NewReadyState)
{
	if (auto PC = GetWorld()->GetFirstPlayerController(); PC && PC->GetPlayerState<ASL_HUBPlayerState>() == this) {  // send data to the LocalPlayerParam
		LocalHUBPlayerParam->SetIsReady(NewReadyState);
		ServerUpdatePlayer(LocalHUBPlayerParam->GetPlayerID(), LocalHUBPlayerParam->GetPlayerName(), LocalHUBPlayerParam->GetIsReady(), LocalHUBPlayerParam->GetChoosenTeam());
	}
}

void ASL_HUBPlayerState::ClientRemoveHUBPlayerParam_Implementation()
{
	ServerRemovePlayerParam(LocalHUBPlayerParam->GetPlayerID());
}

void ASL_HUBPlayerState::ServerSetNewArrival_Implementation(const FString& PlayerParamID, const FString& PlayerName, const bool IsReady, const int ChoosenTeam)
{
	if (auto GM = GetWorld()->GetAuthGameMode<ASL_HUBGameModeBase>(); GM)
		GM->SetNewArrival(PlayerParamID, PlayerName, IsReady, ChoosenTeam);
}

void ASL_HUBPlayerState::ServerUpdatePlayer_Implementation(const FString& PlayerParamID, const FString& PlayerName, const bool IsReady, const int ChoosenTeam)
{
	if (auto GM = GetWorld()->GetAuthGameMode<ASL_HUBGameModeBase>(); GM)
		GM->UpdatePlayer(PlayerParamID, PlayerName, IsReady, ChoosenTeam);
}

void ASL_HUBPlayerState::ServerRemovePlayerParam_Implementation(const FString& RemovePlayerID)
{
	if (auto GM = GetWorld()->GetAuthGameMode<ASL_HUBGameModeBase>(); GM)
		GM->RemovePlayer(RemovePlayerID);
}

void ASL_HUBPlayerState::ClientRefreshPlayerInfo_Implementation(const TArray<FString>& PlayerMessage)
{
	if (auto PC = GetWorld()->GetFirstPlayerController(); PC) {
		if (auto HUD = PC->GetHUD<IStatInfoInterface>(); HUD) {
			// convert TArray to TMap
			TMap<FString, FString> Infos;
			for (int loop = 0; loop < PlayerMessage.Num(); loop += 2) {
				Infos.Add(PlayerMessage[loop], PlayerMessage[loop+1]);
			}

			HUD->OnStatsInfoCleanOrder();
			HUD->OnStatsInfoReceived(Infos);
		}
	}
}