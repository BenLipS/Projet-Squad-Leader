// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Templates/Tuple.h"

#include "StatInfoInterface.generated.h"

UINTERFACE(MinimalAPI)
class UStatInfoInterface : public UInterface
{
	GENERATED_BODY()
};


class SQUADLEADER_API IStatInfoInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnStatInfoReceived(FString Key, FString Value) = 0;

	UFUNCTION()
	virtual void OnStatsInfoReceived(TMap<FString, FString> statsIn) = 0;
	
	UFUNCTION()
	virtual void OnStatsInfoCleanOrder() = 0;
};