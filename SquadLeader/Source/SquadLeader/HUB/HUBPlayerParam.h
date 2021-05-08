// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/Info.h"
#include "Net/UnrealNetwork.h"
#include "HUBPlayerParam.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AHUBPlayerParam : public AInfo
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AHUBPlayerParam();
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	FString PlayerID;
	UPROPERTY(Replicated)
	FString PlayerName;
	UPROPERTY(Replicated)
	bool IsReady;
	UPROPERTY(Replicated)
	int ChoosenTeam;
};
