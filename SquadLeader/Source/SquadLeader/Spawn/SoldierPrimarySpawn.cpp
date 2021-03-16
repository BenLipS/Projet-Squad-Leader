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
}


void ASoldierPrimarySpawn::Destroyed()
{
	if (GetLocalRole() == ROLE_Authority && teamOwner) {
		teamOwner.GetDefaultObject()->RemoveSpawn(this);
	}
	Super::Destroyed();
}


void ASoldierPrimarySpawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASoldierPrimarySpawn, teamOwner);
}


void ASoldierPrimarySpawn::PreInitialisation()
{
	UpdateTeamOwner();
}

int ASoldierPrimarySpawn::getpriority()
{
	return 1;
}


// Called every frame
void ASoldierPrimarySpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASoldierPrimarySpawn::OnRep_ChangeTeamOwner() {
	if (GetLocalRole() == ROLE_Authority) {
		UpdateTeamOwner();
	}
	else {
		ServerChangeTeamOwner(teamOwner);
	}
}

void ASoldierPrimarySpawn::ServerChangeTeamOwner_Implementation(TSubclassOf<ASoldierTeam> _teamOwner)
{
	teamOwner = _teamOwner;
}

bool ASoldierPrimarySpawn::ServerChangeTeamOwner_Validate(TSubclassOf<ASoldierTeam> _teamOwner)
{
	return true;
}

void ASoldierPrimarySpawn::UpdateTeamOwner() {
	if (GetLocalRole() == ROLE_Authority && teamOwner && teamOwner != previousTeamOwner) {  // server only and if teamOwner exist and is changed
		teamOwner.GetDefaultObject()->AddSpawn(this);
		if (previousTeamOwner) previousTeamOwner.GetDefaultObject()->RemoveSpawn(this);
		previousTeamOwner = teamOwner;
	}
}