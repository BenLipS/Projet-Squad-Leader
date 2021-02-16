// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ControlArea/ControlArea.h"
#include "SquadLeaderGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASquadLeaderGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;
	ASquadLeaderGameModeBase();


public:
	// storage of global data for all the game
	TArray<class AControlArea*> controlAreaCollection;
};
