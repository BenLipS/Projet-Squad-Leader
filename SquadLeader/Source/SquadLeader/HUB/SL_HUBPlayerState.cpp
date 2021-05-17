#include "SL_HUBPlayerState.h"
#include "SL_HUBGameModeBase.h"
#include "GameFramework/HUD.h"
#include "../UI/Interface/StatInfoInterface.h"
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

		ServerSetNewArrival(LocalHUBPlayerParam);
	}
}

void ASL_HUBPlayerState::ChangeTeam()
{
	int NewTeamId = (LocalHUBPlayerParam->GetChoosenTeam() % 2) + 1;
	if (GetPawn()->IsLocallyControlled()) {  // send data to the LocalPlayerParam
		LocalHUBPlayerParam->SetChoosenTeam(NewTeamId);
		ServerUpdatePlayer(LocalHUBPlayerParam);

		auto GI = GetGameInstance<USquadLeaderGameInstance>();
		GI->PlayerParam.GetDefaultObject()->SetTeam(NewTeamId);
	}
}

void ASL_HUBPlayerState::ChangeReadyState()
{
	bool NewReadyState = !LocalHUBPlayerParam->GetIsReady();
	if (GetPawn()->IsLocallyControlled()) {  // send data to the LocalPlayerParam
		LocalHUBPlayerParam->SetChoosenTeam(NewReadyState);
		ServerUpdatePlayer(LocalHUBPlayerParam);
	}
}

void ASL_HUBPlayerState::ClientRemoveHUBPlayerParam_Implementation()
{
	ServerRemovePlayerParam(LocalHUBPlayerParam->GetPlayerID());
}

void ASL_HUBPlayerState::ServerSetNewArrival_Implementation(AHUBPlayerParam* NewPlayer)
{
	if (auto GM = GetWorld()->GetAuthGameMode<ASL_HUBGameModeBase>(); GM)
		GM->SetNewArrival(NewPlayer);
}

void ASL_HUBPlayerState::ServerUpdatePlayer_Implementation(AHUBPlayerParam* PlayerParam)
{
	if (auto GM = GetWorld()->GetAuthGameMode<ASL_HUBGameModeBase>(); GM)
		GM->UpdatePlayer(PlayerParam);
}

void ASL_HUBPlayerState::ServerRemovePlayerParam_Implementation(const FString& RemovePlayerID)
{
	if (auto GM = GetWorld()->GetAuthGameMode<ASL_HUBGameModeBase>(); GM)
		GM->RemovePlayer(RemovePlayerID);
}

void ASL_HUBPlayerState::ClientRefreshPlayerInfo_Implementation(const TArray<AHUBPlayerParam*>& PlayerParam)
{
	if (auto PC = GetWorld()->GetFirstPlayerController(); PC) {
		if (auto HUD = PC->GetHUD<IStatInfoInterface>(); HUD) {
			TMap<FString, FString> contentToPrint = GetInfoAsStringPair();

			HUD->OnStatsInfoCleanOrder();
			HUD->OnStatsInfoReceived(contentToPrint);
		}
	}
}

TMap<FString, FString> ASL_HUBPlayerState::GetInfoAsStringPair()
{
	TMap<FString, FString> Infos;
	for (auto& player : PlayerParamList) {
		if (player)
			Infos.Add(FString::FromInt(player->GetIsReady()) + " " + player->GetPlayerName(), FString::FromInt(player->GetChoosenTeam()));
	}
	return Infos;
}