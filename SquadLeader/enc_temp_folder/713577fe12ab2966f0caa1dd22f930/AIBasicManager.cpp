// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBasicManager.h"

void UAIBasicManager::Init(TSubclassOf<ASoldierTeam> _Team, UObject* WorldContextObject)
{
	Team = _Team;

	/*For now Each AIBasicManager Spawn with 4 AIs*/
	FTransform LocationAI{ {0.f, 0.f, 1000.f} , {0.f, 0.f, 0.f} };

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // La maniere de faire le respawn
	ASoldierAI* SquadAI = GEngine->GetWorldFromContextObject(WorldContextObject)->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (SquadAI) {
		SquadAI->SpawnDefaultController();
		SquadAI->FinishSpawning(LocationAI);
	}
}
