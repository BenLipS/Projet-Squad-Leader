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
	TSubclassOf<AAIBasicManager> AIBasicManagerClass;

public:

	UPROPERTY()
	AAIBasicManager* AIBasicManagerTeam1;

	UPROPERTY()
	AAIBasicManager* AIBasicManagerTeam2;

	UPROPERTY()
	TArray<AAISquadManager*> ListAISquadManagers;

	USquadLeaderGameInstance();

	UFUNCTION()
	void InitAIManagers();

	/*
	* For AI placed via drag and drop
	*/
	UFUNCTION()
	void AddAIBasicToManager(AAIBasicController* AIBasic);

	UFUNCTION()
	TArray<AAISquadManager*> GetSquadManagers() { return ListAISquadManagers; }

};
