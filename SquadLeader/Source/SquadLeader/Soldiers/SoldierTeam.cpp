// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierTeam.h"
#include "../SquadLeaderGameModeBase.h"


ASoldierTeam::ASoldierTeam() {
	bReplicates = true;
}


void ASoldierTeam::BeginPlay() {
	Super::BeginPlay();

	// notify the GameMode of the team existence
	if (GetLocalRole() == ROLE_Authority) {
		//auto gameMode = static_cast<ASquadLeaderGameModeBase*>(GetWorld()->GetAuthGameMode());
		//gameMode->SoldierTeamCollection.Add(this);
	}
}


void ASoldierTeam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(ASoldierTeam, ...);
}


void ASoldierTeam::AddControlArea(AControlArea* newControlArea)
{
	CleanControlArea();
	takenControlArea.AddUnique(newControlArea);
}


void ASoldierTeam::RemoveControlArea(AControlArea* newControlArea)
{
	if (takenControlArea.Find(newControlArea))
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
	if (mainSpawnPoints.Find(newSpawn))
		mainSpawnPoints.Remove(newSpawn);
}


void ASoldierTeam::CleanSpawnPoints()
{
	mainSpawnPoints.RemoveAll([](ASoldierSpawn* element) {return element == nullptr; });
}