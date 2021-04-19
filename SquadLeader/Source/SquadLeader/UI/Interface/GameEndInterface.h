// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameEndInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameEndInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IGameEndInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnGameEnd(FString EndMessage) = 0;
};
