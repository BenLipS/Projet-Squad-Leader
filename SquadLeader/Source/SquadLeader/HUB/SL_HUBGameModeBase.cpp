#include "SL_HUBGameModeBase.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameSession.h"
#include "Net/OnlineEngineInterface.h"


void ASL_HUBGameModeBase::TeleportAllPlayersToGame() {
	GetWorld()->ServerTravel("Factory_V1");
}

void ASL_HUBGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// Test if the max number of player is reached
	auto AlreadyConnectedPlayerNumber = GameState->PlayerArray.Num();
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

	// send a message to remove the match in the match-macking system
	if (AlreadyConnectedPlayerNumber >= 6) {
		// GetGameInstance<USquadLeaderGameInstance>()->RemoveGameOnServer()  // TODO Thomas Ba
	}

	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
}

void ASL_HUBGameModeBase::Logout(AController* Exiting)
{
	// notifies that a player has left
	if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("A player left the game."));  // message for the server only
	// TODO: ensure that the player is correctly destroy

	// do the basic job
	Super::Logout(Exiting);
}