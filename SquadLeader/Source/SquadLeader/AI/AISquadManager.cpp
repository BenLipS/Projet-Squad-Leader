// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquadManager.h"

void UAISquadManager::Init(TSubclassOf<ASoldierTeam> _Team, ASoldierPlayer* _Player, UObject* WorldContextObject)
{
	Team = _Team;
	Leader = _Player;
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Init Squad Manager for: %s"), *Leader->GetName()));

	FTransform PlayerTransform = Leader->GetTransform();
	FVector Offset_{ 300.f,0.f,0.f };
	FVector Offset_1{ 0.f,300.f,0.f };
	FTransform LocationAI = PlayerTransform;
	FTransform LocationAI1 = PlayerTransform;
	FTransform LocationAI2 = PlayerTransform;
	LocationAI.SetLocation(PlayerTransform.GetLocation() + Offset_1);
	LocationAI1.SetLocation(PlayerTransform.GetLocation() - Offset_1);
	LocationAI2.SetLocation(PlayerTransform.GetLocation() + Offset_);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // La maniere de faire le respawn
	ASoldierAI* SquadAI = GEngine->GetWorldFromContextObject(WorldContextObject)->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (SquadAI) {
		SquadAI->SpawnDefaultController();
		SquadAI->FinishSpawning(LocationAI);
		AISquadList.Add(Cast<AAISquadController>(SquadAI->Controller));
	}
	ASoldierAI* SquadAI1 = GEngine->GetWorldFromContextObject(WorldContextObject)->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI1, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (SquadAI1) {
		SquadAI1->SpawnDefaultController();
		SquadAI1->FinishSpawning(LocationAI1);
		AISquadList.Add(Cast<AAISquadController>(SquadAI1->Controller));
	}
	ASoldierAI* SquadAI2 = GEngine->GetWorldFromContextObject(WorldContextObject)->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI2, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (SquadAI2) {
		SquadAI2->SpawnDefaultController();
		SquadAI2->FinishSpawning(LocationAI2);
		AISquadList.Add(Cast<AAISquadController>(SquadAI2->Controller));
	}
}
