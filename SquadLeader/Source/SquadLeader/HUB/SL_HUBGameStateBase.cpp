#include "SL_HUBGameStateBase.h"
#include "../SquadLeaderGameInstance.h"
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
}

void ASL_HUBGameStateBase::RefreshPlayerInfo_Implementation()
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
		Infos.Add(FString::FromInt(player->GetIsReady()) + " " +player->GetPlayerName(), FString::FromInt(player->GetChoosenTeam()));
	}
	return Infos;
}

void ASL_HUBGameStateBase::SetNewArrival_Implementation(AHUBPlayerParam* NewPlayer)
{
	for (auto player : PlayersInfo) {
		if (player->GetPlayerID() == NewPlayer->GetPlayerID()) {
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

void ASL_HUBGameStateBase::RemovePlayer_Implementation(const FString& PlayerID)
{
	PlayersInfo.RemoveAll([PlayerID](AHUBPlayerParam* player) {
		return player->GetPlayerID() == PlayerID;
		});
	RefreshPlayerInfo();
}

void ASL_HUBGameStateBase::UpdatePlayer_Implementation(AHUBPlayerParam* PlayerParam)
{
	for (auto player : PlayersInfo) {
		if (player->GetPlayerID() == PlayerParam->GetPlayerID()) {
			player->SetPlayerName(PlayerParam->GetPlayerName());
			player->SetIsReady(PlayerParam->GetIsReady());
			player->SetChoosenTeam(PlayerParam->GetChoosenTeam());

			RefreshPlayerInfo();
		}
	}
}
