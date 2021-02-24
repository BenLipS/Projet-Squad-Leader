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