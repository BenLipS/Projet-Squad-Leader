// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadLeaderGameState.h"

ASquadLeaderGameState::ASquadLeaderGameState()
{
	bReplicates = true;
}

void ASquadLeaderGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME_CONDITION_NOTIFY(ASquadLeaderGameState, test, COND_None, REPNOTIFY_Always);
}
