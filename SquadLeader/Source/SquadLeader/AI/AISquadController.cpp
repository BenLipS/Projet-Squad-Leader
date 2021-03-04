// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquadController.h"
#include "../Spawn/SoldierSpawn.h"

FVector AAISquadController::GetRespawnPoint()  // TODO : Change this function to adapt the squad AI respawn
{
	if (ASoldier* soldier = Cast<ASoldier>(GetPawn()); soldier) {
		if (soldier->GetTeam()) {
			auto AvailableSpawnPoints = soldier->GetTeam().GetDefaultObject()->GetUsableSpawnPoints();
			if (AvailableSpawnPoints.Num() > 0) {

				FVector OptimalPosition = AvailableSpawnPoints[0]->GetActorLocation();
				auto CalculateMinimalDistance = [](FVector PlayerPos, FVector FirstPoint, FVector SecondPoint) {  // return true if the first point is closest
					float dist1 = FVector::Dist(PlayerPos, FirstPoint);
					float dist2 = FVector::Dist(PlayerPos, SecondPoint);
					return dist1 < dist2;
				};

				for (auto loop : AvailableSpawnPoints) {
					if (CalculateMinimalDistance(soldier->GetActorLocation(), loop->GetActorLocation(), OptimalPosition)) {
						OptimalPosition = loop->GetActorLocation();
					}
				}

				return OptimalPosition;
			}
		}
	}
	return FVector(0.f, 0.f, 1500.f); // else return default
}
