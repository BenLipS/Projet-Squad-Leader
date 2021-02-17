// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierSpawn.h"


ASoldierSpawn::ASoldierSpawn() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}


void ASoldierSpawn::BeginPlay() {
	Super::BeginPlay();
}


void ASoldierSpawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


// Called every frame
void ASoldierSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}