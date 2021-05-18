#include "SL_HUBGameModeBase.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameSession.h"
#include "HUBPlayerParam.h"
#include "SL_HUBPlayerState.h"
#include "../SquadLeaderGameInstance.h"
#include "Net/OnlineEngineInterface.h"


void ASL_HUBGameModeBase::TeleportAllPlayersToGame() {
	// close the matchmaking by calling it on the local gameInstance
	if (auto GI = GetGameInstance<USquadLeaderGameInstance>(); GI) {
		GI->CloseMatchMaking();
	}
	
	GetWorld()->ServerTravel("Factory_V2");
}

void ASL_HUBGameModeBase::RefreshPlayerInfo()
{
	auto message = GetInfoAsStringPair(PlayersInfo);
	for (auto PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; PCIterator++)
	{
		auto test = PCIterator->Get();
		auto PS = test->GetPlayerState<ASL_HUBPlayerState>();
		if (PS)
			PS->ClientRefreshPlayerInfo(message);
	}
}

TArray<FString> ASL_HUBGameModeBase::GetInfoAsStringPair(const TArray<AHUBPlayerParam*>& PlayerParam)
{
	TArray<FString> Infos;
	for (auto& player : PlayerParam) {
		if (player) {
			Infos.Add(FString::FromInt(player->GetIsReady()) + " " + player->GetPlayerName());
			Infos.Add(FString::FromInt(player->GetChoosenTeam()));
		}
	}
	return Infos;
}


void ASL_HUBGameModeBase::SetNewArrival(const FString& PlayerID, const FString& PlayerName, const bool IsReady, const int ChoosenTeam)
{
	for (auto player : PlayersInfo) {
		if (player && player->GetPlayerID() == PlayerID) {
			return;
		}
	}
	AHUBPlayerParam* NewEntry = NewObject<AHUBPlayerParam>();
	NewEntry->SetPlayerId(PlayerID);
	NewEntry->SetPlayerName(PlayerName);
	NewEntry->SetIsReady(IsReady);
	NewEntry->SetChoosenTeam(ChoosenTeam);
	PlayersInfo.Add(NewEntry);
	RefreshPlayerInfo();
}

void ASL_HUBGameModeBase::UpdatePlayer(const FString& PlayerID, const FString& PlayerName, const bool IsReady, const int ChoosenTeam)
{
	for (auto player : PlayersInfo) {
		if (player && player->GetPlayerID() == PlayerID) {
			player->SetPlayerName(PlayerName);
			player->SetIsReady(IsReady);
			player->SetChoosenTeam(ChoosenTeam);

			RefreshPlayerInfo();
		}
	}

	TestReadyness();
}

void ASL_HUBGameModeBase::RemovePlayer(const FString& PlayerID)
{
	PlayersInfo.RemoveAll([PlayerID](AHUBPlayerParam* player) {
		return player->GetPlayerID() == PlayerID;
		});
	RefreshPlayerInfo();
}

void ASL_HUBGameModeBase::TestReadyness()
{
	bool EverythingReady = true;
	for (auto player : PlayersInfo) {
		EverythingReady = player->GetIsReady() && EverythingReady;  // test all player readyness
	}

	// test if teams are balance

	if (EverythingReady) {
		if (LastTestWasReady) {
			TeleportAllPlayersToGame();
		}
		else {  // retest and save this test
			GetWorldTimerManager().SetTimer(timerTestReadyness, this,
				&ASL_HUBGameModeBase::TestReadyness, WaitingTimeMenu);
			LastTestWasReady = true;
		}
	}
	else {  // clean timer
		GetWorld()->GetTimerManager().ClearTimer(timerTestReadyness);
		LastTestWasReady = false;
	}
}


void ASL_HUBGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// Test if the max number of player is reached
	int AlreadyConnectedPlayerNumber = GameState->PlayerArray.Num();
	if (AlreadyConnectedPlayerNumber < NbMaxPlayer)
	{
		// Login unique id must match server expected unique id type OR No unique id could mean game doesn't use them
		const bool bUniqueIdCheckOk = (!UniqueId.IsValid() || (UniqueId.GetType() == UOnlineEngineInterface::Get()->GetDefaultOnlineSubsystemName()));
		if (bUniqueIdCheckOk)
		{
			ErrorMessage = GameSession->ApproveLogin(Options);
		}
		else
		{
			ErrorMessage = TEXT("incompatible_unique_net_id");
		}
	}
	else
	{
		ErrorMessage = TEXT("max_number_of_player_connected");
	}
	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
}

void ASL_HUBGameModeBase::Logout(AController* Exiting)
{
	// notifies that a player has left
	if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("A player left the game."));  // message for the server only
	// TODO: ensure that the player is correctly destroy
	// RemovePlayer(Exiting->GetPlayerState<ASL_HUBPlayerState>()->Get);

	// do the basic job
	Super::Logout(Exiting);
}