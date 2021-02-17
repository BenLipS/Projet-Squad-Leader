// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierTeam.h"


ASoldierTeam::ASoldierTeam() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}


void ASoldierTeam::BeginPlay() {
	Super::BeginPlay();
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