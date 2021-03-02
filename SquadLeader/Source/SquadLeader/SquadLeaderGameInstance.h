// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AI/AIBasicManager.h"
#include "AI/AISquadManager.h"
#include "AI/AIBasicController.h"
#include "SquadLeaderGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API USquadLeaderGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAIBasicManager> AIBasicManagerClass;

	UPROPERTY()
	UAIBasicManager* AIBasicManagerTeam1;

	UPROPERTY()
	UAIBasicManager* AIBasicManagerTeam2;

	UPROPERTY()
	TArray<UAISquadManager*> ListAISquadManagers;


public:
	USquadLeaderGameInstance();

	UFUNCTION()
	void InitAIManagers();

	/*
	* For AI placed via drag and drop
	*/
	UFUNCTION()
	void AddAIBasicToManager(AAIBasicController* AIBasic);

	UFUNCTION()
	TArray<UAISquadManager*> GetSquadManagers() { return ListAISquadManagers; }
};
