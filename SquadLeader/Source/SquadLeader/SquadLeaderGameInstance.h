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

	UPROPERTY()
	UAIBasicManager* AIBasicManagerTeam1;

	UPROPERTY()
	UAIBasicManager* AIBasicManagerTeam2;

	UPROPERTY()
	TArray<UAISquadManager*> ListAISquadManagers;


public:
	UFUNCTION()
	void InitAIManagers();

	void AddAIBasicToManager(AAIBasicController* AIBasic);


};
