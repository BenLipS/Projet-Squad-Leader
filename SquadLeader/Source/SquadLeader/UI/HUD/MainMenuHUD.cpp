// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHUD.h"

#include "SquadLeader/SquadLeaderGameInstance.h"


void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();
	InitHUD();
	GetProfileData();
	OnGameInfoReceived("Salut mon pote !", "Oscar", "");
	OnGameInfoReceived("C'est pas si mal.", "Simon", "");
	OnGameInfoReceived("Breizh mon amour !", "Thomas", "");
	OnGameInfoReceived("Tigre prout LOL.", "Fabien", "");
}

void AMainMenuHUD::GetProfileData()
{
	GetGameInstance<USquadLeaderGameInstance>()->ProfileInfo();
}