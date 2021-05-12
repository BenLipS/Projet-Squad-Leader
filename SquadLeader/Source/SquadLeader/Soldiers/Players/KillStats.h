// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/Info.h"
#include "Net/UnrealNetwork.h"
#include "KillStats.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AKillStats : public AInfo
{
	GENERATED_BODY()

public:
	int NbDeathByAI = 0;
	int NbDeathByPlayer = 0;
	int NbKillAI = 0;
	int NbKillPlayer = 0;
};
