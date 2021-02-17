// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierSecondarySpawn.h"


ASoldierSecondarySpawn::ASoldierSecondarySpawn() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}


void ASoldierSecondarySpawn::BeginPlay() {
	Super::BeginPlay();
	canBeUsed = initialCanBeUsed;
}


void ASoldierSecondarySpawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASoldierSecondarySpawn, TeamOwner);
}


// Called every frame
void ASoldierSecondarySpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}