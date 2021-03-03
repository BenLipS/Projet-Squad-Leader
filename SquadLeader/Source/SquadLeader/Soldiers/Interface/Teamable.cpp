// Fill out your copyright notice in the Description page of Project Settings.


#include "Teamable.h"
#include "../SoldierTeam.h"

// Add default functionality here for any ITeamable functions that are not pure virtual.

ASoldierTeam* ITeamable::GetTeam()
{
	return Team;
}

bool ITeamable::SetTeam(ASoldierTeam* _Team)
{
	Team = _Team;
	return true;
}