#include "SoldierPrimarySpawn.h"
#include "Camera/CameraActor.h"

ASoldierPrimarySpawn::ASoldierPrimarySpawn() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	canBeUsed = true;

	CameraActor = CreateDefaultSubobject<ACameraActor>(TEXT("Camera Actor"));
}


void ASoldierPrimarySpawn::BeginPlay() {
	Super::BeginPlay();
}


void ASoldierPrimarySpawn::Destroyed()
{
	if (GetLocalRole() == ROLE_Authority && teamOwner) {
		teamOwner->RemoveSpawn(this);
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

int ASoldierPrimarySpawn::GetPriority() const
{
	return 2;
}

void ASoldierPrimarySpawn::OnRep_ChangeTeamOwner() {
	if (GetLocalRole() == ROLE_Authority) {
		UpdateTeamOwner();
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

void ASoldierPrimarySpawn::UpdateTeamOwner() {
	if (GetLocalRole() == ROLE_Authority && teamOwner && teamOwner != previousTeamOwner) {  // server only and if teamOwner exist and is changed
		teamOwner->AddSpawn(this);
		if (previousTeamOwner) previousTeamOwner->RemoveSpawn(this);
		previousTeamOwner = teamOwner;
	}
}