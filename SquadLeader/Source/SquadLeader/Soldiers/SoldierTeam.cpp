// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierTeam.h"
#include "../SquadLeaderGameModeBase.h"
#include "Soldier.h"
#include "../Spawn/SoldierSpawn.h"
#include "../ControlArea/ControlArea.h"


ASoldierTeam::ASoldierTeam() {
	bReplicates = true;
}


void ASoldierTeam::BeginPlay() {
	Super::BeginPlay();
}


void ASoldierTeam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoldierTeam, soldierList);
	DOREPLIFETIME(ASoldierTeam, takenControlArea);
	DOREPLIFETIME(ASoldierTeam, mainSpawnPoints);

}


void ASoldierTeam::AddSoldierList(ASoldier* newSoldier)
{
	soldierList.AddUnique(newSoldier);
}

void ASoldierTeam::RemoveSoldierList(ASoldier* newSoldier)
{
	if (soldierList.Contains(newSoldier))
		soldierList.Remove(newSoldier);
}

void ASoldierTeam::CleanSoldierList()
{
	soldierList.RemoveAll([](ASoldier* element) {return element == nullptr; });
}


void ASoldierTeam::AddControlArea(AControlArea* newControlArea)
{
	takenControlArea.AddUnique(newControlArea);
}

void ASoldierTeam::RemoveControlArea(AControlArea* newControlArea)
{
	if (takenControlArea.Contains(newControlArea))
		takenControlArea.Remove(newControlArea);
}

void ASoldierTeam::CleanControlArea()
{
	takenControlArea.RemoveAll([](AControlArea* element) {return element == nullptr; });
}


void ASoldierTeam::AddSpawn(ASoldierSpawn* newSpawn)
{
	mainSpawnPoints.AddUnique(newSpawn);
}

void ASoldierTeam::RemoveSpawn(ASoldierSpawn* newSpawn)
{
	if (mainSpawnPoints.Contains(newSpawn))
		mainSpawnPoints.Remove(newSpawn);
}

void ASoldierTeam::CleanSpawnPoints()
{
	mainSpawnPoints.RemoveAll([](ASoldierSpawn* element) {return element == nullptr; });
}

TArray<ASoldierSpawn*> ASoldierTeam::GetUsableSpawnPoints()
{
	TArray<ASoldierSpawn*>result{};
	for (auto element : mainSpawnPoints) {
		if (element->GetUsable())
			result.Add(element);
	}

	return result;
}