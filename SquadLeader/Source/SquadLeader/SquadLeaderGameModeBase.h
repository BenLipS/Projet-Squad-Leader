// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SquadLeaderGameModeBase.generated.h"

UCLASS()
class SQUADLEADER_API ASquadLeaderGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;
	ASquadLeaderGameModeBase();

	void InitGameWithGameState();

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Respawn")
		float RespawnDelay;
	
public:
	void SoldierDied(AController* _Controller);
	void RespawnSoldier(AController* _Controller);
	void CheckControlAreaVictoryCondition();
	void EndGame();
};
