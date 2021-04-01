// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AI/AIBasicManager.h"
#include "AI/AISquadManager.h"
#include "AI/AIBasicController.h"
#include "AI/InfluenceMap/InfluenceMapGrid.h"
#include "SquadLeaderGameModeBase.generated.h"

UCLASS()
class SQUADLEADER_API ASquadLeaderGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;
	ASquadLeaderGameModeBase();

	void InitGameWithGameState();

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void Logout(AController* Exiting) override;

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GameData")
		int NbMaxPlayer = 6;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Respawn")
		float RespawnDelay;
	
public:
	void SoldierDied(AController* _Controller);
	void RespawnSoldier(AController* _Controller);
	void CheckControlAreaVictoryCondition();
	void CheckTeamTicketsVictoryCondition();
	void EndGame();

protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AAIBasicManager> AIBasicManagerClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AInfluenceMapGrid> InfluenceMapClass;

public:

	UPROPERTY()
		TMap<class ASoldierTeam*, AAIBasicManager*> AIBasicManagerCollection;
	UFUNCTION()
		void InitAIManagers();

	UPROPERTY()
		TArray<AAISquadManager*> ListAISquadManagers;

	UPROPERTY()
		AInfluenceMapGrid* InfluenceMap;
	UFUNCTION()
		void InitInfluenceMap();

	/*
	* For AI placed via drag and drop
	*/
	UFUNCTION()
		void AddAIBasicToManager(AAIBasicController* AIBasic);

	UFUNCTION()
		TArray<AAISquadManager*> GetSquadManagers() { return ListAISquadManagers; }
};
