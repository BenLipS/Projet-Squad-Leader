// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AI/AIBasicManager.h"
#include "AI/AISquadManager.h"
#include "AI/AIBasicController.h"
#include "AI/InfluenceMap/InfluenceMapGrid.h"
#include "SquadLeaderGameModeBase.generated.h"

class ASoldier;

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
	void EndGame(ASoldierTeam* WinningTeam);
	void CloseGame();

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

//////////////// EXP Rules
public:
	void GrantOverTimeEXPToSoldier();
	void NotifySoldierKilled(ASoldier* _DeadSoldier, ASoldier* _Killer);
	void NotifyControlAreaCaptured(AControlArea* _ControlArea);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "EXP Soldiers|Time")
	float TimeBetweenGrantedEXP = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "EXP Soldiers")
	float EXP_Kill = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "EXP Soldiers")
	float EXP_KillSquad = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "EXP Soldiers")
	float EXP_KillAssist = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "EXP Soldiers")
	float EXP_Death = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "EXP Soldiers")
	float EXP_ControlAreaCaptured = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "EXP Soldiers|Time")
	float EXP_OverTime = 5.f;

public:
	float GetEXP_Kill() const { return EXP_Kill; }
	float GetEXP_KillSquad() const { return EXP_KillSquad; }
	float GetEXP_KillAssist() const { return EXP_KillAssist; }
	float GetEXP_Death() const { return EXP_Death; }
	float GetEXP_ControlAreaCaptured() const { return EXP_ControlAreaCaptured; }
	float GetEXP_OverTime() const { return EXP_OverTime; }
};
