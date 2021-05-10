// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MenuGameInfoInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuGameInfoInterface : public UInterface
{
	GENERATED_BODY()
};

class SQUADLEADER_API IMenuGameInfoInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnGameInfoReceived(FString Key, FString Value, FString IP) = 0;

	UFUNCTION()
	virtual void OnGamesInfoCleanOrder() = 0;
};
