// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadLeaderGameState.h"

ASquadLeaderGameState::ASquadLeaderGameState()
{
	bReplicates = true;
}

void ASquadLeaderGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ASquadLeaderGameState, ControlAreaManager, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ASquadLeaderGameState, SoldierTeamCollection, COND_None, REPNOTIFY_Always);
}

void ASquadLeaderGameState::AddSoldierTeam(ASoldierTeam* _SoldierTeam)
{
	SoldierTeamCollection.AddUnique(_SoldierTeam);
}

TArray<ASoldierTeam*> ASquadLeaderGameState::GetSoldierTeamCollection()
{
	return SoldierTeamCollection;
}

void ASquadLeaderGameState::SetControlAreaManager(AControlAreaManager* _ControlAreaManager)
{
	ControlAreaManager = _ControlAreaManager;
}

AControlAreaManager* ASquadLeaderGameState::GetControlAreaManager()
{
	return ControlAreaManager;
}
