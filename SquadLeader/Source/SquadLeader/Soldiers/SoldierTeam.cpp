// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierTeam.h"
#include "../SquadLeaderGameModeBase.h"


ASoldierTeam::ASoldierTeam() {

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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


// Called every frame
void ASoldierTeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ASoldierTeam::AddSpawn(ASoldierSpawn* newSpawn)
{
	CleanSpawnPoints();
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