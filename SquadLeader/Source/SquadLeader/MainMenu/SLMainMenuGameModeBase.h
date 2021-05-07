// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameParam/GameParam.h"
#include "GameFramework/GameModeBase.h"
#include "SLMainMenuGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASLMainMenuGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;

	// basic games params
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <AGameParam> DefaultGameParam;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <AGameParam> MinGameParam;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <AGameParam> MaxGameParam;

	//UGameParam CreateRandomGameParam();
};
