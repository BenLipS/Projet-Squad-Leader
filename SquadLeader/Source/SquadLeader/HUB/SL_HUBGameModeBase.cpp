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
	for (auto PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; PCIterator++)
	{
		PCIterator->Get()->GetPlayerState<ASL_HUBPlayerState>()->ClientRefreshPlayerInfo(PlayersInfo);
	}
}

void ASL_HUBGameModeBase::SetNewArrival(AHUBPlayerParam* NewPlayer)
{
	for (auto player : PlayersInfo) {
		if (player && player->GetPlayerID() == NewPlayer->GetPlayerID()) {
			return;
		}
	}
	AHUBPlayerParam* NewEntry = NewObject<AHUBPlayerParam>();
	NewEntry->SetPlayerId(NewPlayer->GetPlayerID());
	NewEntry->SetPlayerName(NewPlayer->GetPlayerName());
	NewEntry->SetIsReady(NewPlayer->GetIsReady());
	NewEntry->SetChoosenTeam(NewPlayer->GetChoosenTeam());
	PlayersInfo.Add(NewEntry);
	RefreshPlayerInfo();
}

void ASL_HUBGameModeBase::UpdatePlayer(AHUBPlayerParam* PlayerParam)
{
	for (auto player : PlayersInfo) {
		if (player && player->GetPlayerID() == PlayerParam->GetPlayerID()) {
			player->SetPlayerName(PlayerParam->GetPlayerName());
			player->SetIsReady(PlayerParam->GetIsReady());
			player->SetChoosenTeam(PlayerParam->GetChoosenTeam());

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
		else {  // retest in 5s and save this test
			GetWorldTimerManager().SetTimer(timerTestReadyness, this,
				&ASL_HUBGameModeBase::TestReadyness, 5.f);
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