// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SquadLeaderGameState.h"
#include "SquadLeaderInitGameState.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASquadLeaderInitGameState : public ASquadLeaderGameState
{
	GENERATED_BODY()

public:
	ASquadLeaderInitGameState();

	void InitMapAndData();
	void InitActorInWorld();
};
