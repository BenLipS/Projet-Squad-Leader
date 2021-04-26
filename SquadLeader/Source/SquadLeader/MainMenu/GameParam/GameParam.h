// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameParam.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API UGameParam : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FString Name;
	UPROPERTY(EditDefaultsOnly)
	int LevelTarget;
	UPROPERTY(EditDefaultsOnly)
	int LevelRange;
	UPROPERTY(EditDefaultsOnly)
	int NbAIBasicAssault;
	UPROPERTY(EditDefaultsOnly)
	int NbAIBasicHeavy;
	UPROPERTY(EditDefaultsOnly)
	int LevelAIBasic;
	UPROPERTY(EditDefaultsOnly)
	int StartingNbAISquad;
	UPROPERTY(EditDefaultsOnly)
	int LevelAISquad;
	UPROPERTY(EditDefaultsOnly)
	int NbTickets;
	UPROPERTY(EditDefaultsOnly)
	int Weather;
	UPROPERTY(EditDefaultsOnly)
	int RespawnDuration;
	UPROPERTY(EditDefaultsOnly)
	bool FriendOnly;
};
