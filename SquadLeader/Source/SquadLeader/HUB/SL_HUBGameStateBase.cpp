#include "SL_HUBGameStateBase.h"
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

void ASL_HUBGameStateBase::RefreshPlayerInfo_Implementation()
{
	if (auto PC = GetWorld()->GetFirstPlayerController(); PC) {
		if (auto HUD = PC->GetHUD<IStatInfoInterface>(); HUD) {
			TMap<FString, FString> contentToPrint= GetInfoAsStringPairs();

			HUD->OnStatsInfoCleanOrder();
			HUD->OnStatsInfoReceived(contentToPrint);
		}
	}
}

TMap<FString, FString> ASL_HUBGameStateBase::GetInfoAsStringPairs()
{
	TMap<FString, FString> Infos;
	for (auto& player : PlayersInfo) {
		Infos.Add(FString::FromInt(player->IsReady) + " " +player->PlayerName, FString::FromInt(player->ChoosenTeam));
	}
	return Infos;
}

void ASL_HUBGameStateBase::SetNewArrival(AHUBPlayerParam* NewPlayer)
{
	for (auto player : PlayersInfo) {
		if (player->PlayerID == NewPlayer->PlayerID) {
			return;
		}
	}
	PlayersInfo.Add(NewPlayer);
	RefreshPlayerInfo();
}

void ASL_HUBGameStateBase::RemovePlayer(FString PlayerID)
{
	PlayersInfo.RemoveAll([PlayerID](AHUBPlayerParam* player) {
		return player->PlayerID == PlayerID;
		});
	RefreshPlayerInfo();
}

void ASL_HUBGameStateBase::UpdatePlayer(AHUBPlayerParam PlayerParam)
{
	for (auto player : PlayersInfo) {
		if (player->PlayerID == PlayerParam.PlayerID) {
			player->PlayerName = PlayerParam.PlayerName;
			player->IsReady = PlayerParam.IsReady;
			player->ChoosenTeam = PlayerParam.ChoosenTeam;

			RefreshPlayerInfo();
		}
	}
}
