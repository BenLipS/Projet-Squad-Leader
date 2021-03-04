// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamableContainer.h"
#include "../SoldierTeam.h"

// Add default functionality here for any ITeamableContainer functions that are not pure virtual.

TSubclassOf<ASoldierTeam> ITeamableContainer::GetTeam()
{
	return Team;
}

bool ITeamableContainer::SetTeam(TSubclassOf<ASoldierTeam> _Team)
{
	Team = _Team;
	return true;
}