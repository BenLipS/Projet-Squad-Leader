// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "../Spawn/SoldierSecondarySpawn.h"
#include "ControlAreaTeamStat.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API AControlAreaTeamStat : public AInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly)
		int controlValue = 0;
	UPROPERTY(EditInstanceOnly)
		int presenceTeam = 0;
	UPROPERTY(EditInstanceOnly)
		ASoldierSecondarySpawn* spawnTeam = nullptr;

	// Sets default values for this actor's properties
	AControlAreaTeamStat() {};

	void ChangeSpawnState(bool state);

	void ChangeSpawnTeam(ASoldierTeam* _teamOwner);
};
