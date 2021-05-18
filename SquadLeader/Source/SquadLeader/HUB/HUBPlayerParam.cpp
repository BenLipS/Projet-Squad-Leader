// Fill out your copyright notice in the Description page of Project Settings.


#include "HUBPlayerParam.h"

AHUBPlayerParam::AHUBPlayerParam()
{
	bReplicates = true;
}

void AHUBPlayerParam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AHUBPlayerParam, PlayerID, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHUBPlayerParam, PlayerName, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHUBPlayerParam, IsReady, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHUBPlayerParam, ChoosenTeam, COND_None, REPNOTIFY_Always);
}


void AHUBPlayerParam::SetPlayerId(FString NewId)
{
	PlayerID = NewId;
}

void AHUBPlayerParam::SetPlayerName(FString NewName)
{
	PlayerName = NewName;
}

void AHUBPlayerParam::SetIsReady(bool Readyness)
{
	IsReady = Readyness;
}

void AHUBPlayerParam::SetChoosenTeam(int TeamId)
{
	ChoosenTeam = TeamId;
}
