#include "SL_HUBGameStateBase.h"
#include "../SquadLeaderGameInstance.h"
#include "SL_HUBGameModeBase.h"
#include "../UI/Interface/StatInfoInterface.h"
#include "../UI/HUD/SL_HUD.h"

ASL_HUBGameStateBase::ASL_HUBGameStateBase()
{
	bReplicates = true;
}

void ASL_HUBGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ASL_HUBGameStateBase, PlayersInfo, COND_None, REPNOTIFY_Always);
}

void ASL_HUBGameStateBase::BeginPlay()
{
	// notify the bdd base that the player have join a new game
	GetGameInstance<USquadLeaderGameInstance>()->ChangeNetworkState(2);

	// ask the server for array content replication
	ClientAskArrayReplication();
}

void ASL_HUBGameStateBase::ClientAskArrayReplication_Implementation()
{
	ServerAskArrayReplication();
}

void ASL_HUBGameStateBase::ClientAskArrayReplication_Implementation()
{
	ServerAskArrayReplication();
}

void ASL_HUBGameStateBase::ServerAskArrayReplication_Implementation()
{
	ClientSyncHUBParamArray(PlayersInfo);
}

void ASL_HUBGameStateBase::ClientSyncHUBParamArray_Implementation(const TArray<AHUBPlayerParam*>& ServerPlayersInfo)
{
	PlayersInfo = ServerPlayersInfo;
}

void ASL_HUBGameStateBase::ClientRefreshPlayerInfo_Implementation()
{
	if (auto PC = GetWorld()->GetFirstPlayerController(); PC) {
		if (auto HUD = PC->GetHUD<IStatInfoInterface>(); HUD) {
			TMap<FString, FString> contentToPrint= GetInfoAsStringPair();

			HUD->OnStatsInfoCleanOrder();
			HUD->OnStatsInfoReceived(contentToPrint);
		}
	}
}

TMap<FString, FString> ASL_HUBGameStateBase::GetInfoAsStringPair()
{
	TMap<FString, FString> Infos;
	for (auto& player : PlayersInfo) {
		if (player)
			Infos.Add(FString::FromInt(player->GetIsReady()) + " " +player->GetPlayerName(), FString::FromInt(player->GetChoosenTeam()));
	}
	return Infos;
}

void ASL_HUBGameStateBase::MulticastSetNewArrival_Implementation(AHUBPlayerParam* NewPlayer)
{
	if (GetLocalRole() == ROLE_Authority) {
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
		ClientRefreshPlayerInfo();
	}
	else {
		ServerSetNewArrival(NewPlayer);
	}
}
void ASL_HUBGameStateBase::ServerSetNewArrival_Implementation(AHUBPlayerParam* NewPlayer)
{
	MulticastSetNewArrival(NewPlayer);
}

void ASL_HUBGameStateBase::MulticastRemovePlayer_Implementation(const FString& PlayerID)
{
	PlayersInfo.RemoveAll([PlayerID](AHUBPlayerParam* player) {
		return player->GetPlayerID() == PlayerID;
		});
	ClientRefreshPlayerInfo();
}

void ASL_HUBGameStateBase::MulticastUpdatePlayer_Implementation(AHUBPlayerParam* PlayerParam)
{
	for (auto player : PlayersInfo) {
		if (player && player->GetPlayerID() == PlayerParam->GetPlayerID()) {
			player->SetPlayerName(PlayerParam->GetPlayerName());
			player->SetIsReady(PlayerParam->GetIsReady());
			player->SetChoosenTeam(PlayerParam->GetChoosenTeam());

			ClientRefreshPlayerInfo();
		}
	}

	if (GetLocalRole() == ROLE_Authority) {  // on server only
		TestReadyness();
	}
}

void ASL_HUBGameStateBase::TestReadyness()
{
	bool EverythingReady = true;
	for (auto player : PlayersInfo) {
		EverythingReady = player->GetIsReady() && EverythingReady;  // test all player readyness
	}

	// test if teams are balance

	if (EverythingReady) {
		if (LastTestWasReady) {
			GetWorld()->GetAuthGameMode<ASL_HUBGameModeBase>()->TeleportAllPlayersToGame();
		}
		else {  // retest in 5s and save this test
			GetWorldTimerManager().SetTimer(timerTestReadyness, this,
				&ASL_HUBGameStateBase::TestReadyness, 5.f);
			LastTestWasReady = true;
		}
	}
	else {  // clean timer
		GetWorld()->GetTimerManager().ClearTimer(timerTestReadyness);
		LastTestWasReady = false;
	}
}
