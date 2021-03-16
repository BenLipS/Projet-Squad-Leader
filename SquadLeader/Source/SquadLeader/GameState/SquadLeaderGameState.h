// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "SquadLeaderGameState.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASquadLeaderGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASquadLeaderGameState();

	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	//UPROPERTY(Replicated)
};
