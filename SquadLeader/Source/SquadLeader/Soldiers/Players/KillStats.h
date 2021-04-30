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
	AKillStats();
	
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(Replicated);
	int NbDeathByAI = 0;
	UPROPERTY(Replicated);
	int NbDeathByPlayer = 0;
	UPROPERTY(Replicated);
	int NbKillAI = 0;
	UPROPERTY(Replicated);
	int NbKillPlayer = 0;
};
