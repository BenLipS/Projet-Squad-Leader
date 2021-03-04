// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ControlArea/ControlAreaManager.h"
#include "Soldiers/SoldierTeam.h"
#include "SquadLeaderGameModeBase.generated.h"

UCLASS()
class SQUADLEADER_API ASquadLeaderGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;
	ASquadLeaderGameModeBase();


public:
	// storage of global data for all the game
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameModeData")
	TSubclassOf<AControlAreaManager> ControlAreaManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameModeData")
	TArray<TSubclassOf<ASoldierTeam>> SoldierTeamCollection;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Respawn")
	float RespawnDelay;
	
public:
	void SoldierDied(AController* _Controller);
	void RespawnSoldier(AController* _Controller);
};
