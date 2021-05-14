// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AI/AIBasicManager.h"
#include "AI/AISquadManager.h"
#include "AI/AIBasicController.h"
#include "AI/InfluenceMap/InfluenceMapGrid.h"
#include "MainMenu/PlayerParam/PlayerParam.h"
#include "SquadLeaderGameModeBase.generated.h"

class ASoldier;

UCLASS()
class SQUADLEADER_API ASquadLeaderGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;
	ASquadLeaderGameModeBase();

	// virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void Logout(AController* Exiting) override;

	APawn* SpawnSoldier(TSubclassOf<APlayerParam> PlayerParam, AController* OwningController);

protected:
	void ChangeGameState();
	void FetchGameParam();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "GameData")
	float Weather = 0;
	UPROPERTY(BlueprintReadOnly, Category = "GameData")
	float RespawnDelay = 5.0f;
	UPROPERTY(BlueprintReadOnly, Category = "GameData")
	int BaseTicketsNumber = 100;
	UPROPERTY(BlueprintReadOnly, Category = "GameData")
	int AIBasicAssaultNumber = 6;
	UPROPERTY(BlueprintReadOnly, Category = "GameData")
	int AIBasicHeavyNumber = 6;
	UPROPERTY(BlueprintReadOnly, Category = "GameData")
	int StartingAISquadNumber = 3;

public:
	int const GetBaseTicketForTeam() {return BaseTicketsNumber;}
	int const GetBaseAIBasicAssaultNumberForTeam() { return AIBasicAssaultNumber; }
	int const GetBaseAIBasicHeavyNumberForTeam() { return AIBasicHeavyNumber; }
	int const GetBaseSquadAINumber() { return StartingAISquadNumber; }
	
public:
	UFUNCTION(BlueprintCallable)
	void RespawnSoldier(ASoldier* _Soldier, AControlArea* _ControlArea = nullptr);

	UFUNCTION(BlueprintCallable)
	void CheckControlAreaVictoryCondition();

	UFUNCTION(BlueprintCallable)
	void CheckTeamTicketsVictoryCondition();

	UFUNCTION(BlueprintCallable)
	void EndGame(ASoldierTeam* WinningTeam);

	UFUNCTION(BlueprintCallable)
	void CloseGame();

//////////////// Respawn
	UFUNCTION()
	void DisplayRespawnHUD(ASoldierPlayer* _SoldierPlayer);

//////////////// AIs
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAIBasicManager> AIBasicManagerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AInfluenceMapGrid> InfluenceMapClass;

	bool IsGameOver = false;

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

//////////////// Notifications
public:
	void NotifySoldierKilled(ASoldier* _DeadSoldier, ASoldier* _KillerSoldier);
	void NotifyControlAreaCaptured(AControlArea* _ControlArea);

protected:
	void UpdateTicketsFromSoldierDeath(ASoldier* _DeadSoldier);
	void StartRespawnTimerForDeadSoldier(ASoldier* _DeadSoldier);
	void NotifySoldierDeathToAllPlayers(ASoldier* _DeadSoldier, ASoldier* _KillerSoldier);

//////////////// Ticket Rules
	UPROPERTY(EditDefaultsOnly, Category = "Tickets")
	int TicketToRemove_Player = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Tickets")
	int TicketToRemove_AISquad = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Tickets")
	int TicketToRemove_AIBasic = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Tickets")
		int FrequenceForControlAreaInfluence = 5;
	UPROPERTY(EditDefaultsOnly, Category = "Tickets")
		int ControlAreaTicketsReduction = 1;

	void CheckControlAreaAdvantage();

//////////////// Killing Streak management
protected:
	UPROPERTY(EditDefaultsOnly, Category = "KillingStreak")
		int NbKillsForBounty = 10;
	UPROPERTY(EditDefaultsOnly, Category = "KillingStreak")
		int BountyValueInTickets = 2;

	void ManageKillingStreak(ASoldier* _DeadSoldier, ASoldier* _Killer);
	void ResetKillingStreak(ASoldierPlayer* _Soldier);
	void IncreaseKillingStreak(ASoldierPlayer* _Soldier);
	void NotifyKillingStreak(ASoldierPlayer* _Soldier);
	void NotifyBounty(ASoldierPlayer* _Soldier);

//////////////// EXP Rules
protected:
	void GrantOverTimeEXPToSoldier();
	void GrantEXPFromSoldierDeath(ASoldier* _DeadSoldier, ASoldier* _KillerSoldier);

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
