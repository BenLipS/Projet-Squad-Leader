// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHUD.h"

#include "SquadLeader/SquadLeaderGameInstance.h"


void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();
	InitHUD();
	GetProfileData();
}

void AMainMenuHUD::GetProfileData()
{
	GetGameInstance<USquadLeaderGameInstance>()->ProfileInfo();
}