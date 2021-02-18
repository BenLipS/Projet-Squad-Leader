// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierPrimarySpawn.h"


ASoldierPrimarySpawn::ASoldierPrimarySpawn() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	canBeUsed = true;
}


void ASoldierPrimarySpawn::BeginPlay() {
	Super::BeginPlay();
	if (teamOwner) {
		teamOwner->AddSpawn(this);
		if (previousTeamOwner) previousTeamOwner->RemoveSpawn(this);
		previousTeamOwner = teamOwner;
	}
}


void ASoldierPrimarySpawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASoldierPrimarySpawn, teamOwner);
}


// Called every frame
void ASoldierPrimarySpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASoldierPrimarySpawn::OnRep_ChangeTeamOwner() {
	if (GetLocalRole() == ROLE_Authority) {
		teamOwner->AddSpawn(this);
		if (previousTeamOwner) previousTeamOwner->RemoveSpawn(this);
		previousTeamOwner = teamOwner;
	}
	else {
		ServerChangeTeamOwner(teamOwner);
	}
}

void ASoldierPrimarySpawn::ServerChangeTeamOwner_Implementation(ASoldierTeam* _teamOwner)
{
	teamOwner = _teamOwner;
}

bool ASoldierPrimarySpawn::ServerChangeTeamOwner_Validate(ASoldierTeam* _teamOwner)
{
	return true;
}