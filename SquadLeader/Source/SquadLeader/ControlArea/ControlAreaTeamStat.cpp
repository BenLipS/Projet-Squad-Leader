// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaTeamStat.h"

void AControlAreaTeamStat::ChangeSpawnState(bool state) {
	if (spawnTeam) {
		spawnTeam->SetUsed(state);
	}
}

void AControlAreaTeamStat::ChangeSpawnTeam(TSubclassOf<ASoldierTeam> _teamOwner) {
	if (spawnTeam) {
		spawnTeam->teamOwner = _teamOwner;
	}
}