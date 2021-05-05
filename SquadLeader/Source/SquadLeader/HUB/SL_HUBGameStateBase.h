// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/GameStateBase.h"
#include "HUBPlayerParam.h"
#include "Net/UnrealNetwork.h"
#include "SL_HUBGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASL_HUBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ASL_HUBGameStateBase();
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	// We know that we will only have 6 players max
	UPROPERTY(Replicated)
	TArray<AHUBPlayerParam*> PlayersInfo;

public:
	UFUNCTION(NetMulticast, unreliable)
	void RefreshPlayerInfo();
	void RefreshPlayerInfo_Implementation();
	TMap<FString, FString> GetInfoAsStringPairs();

	void SetNewArrival(AHUBPlayerParam* NewPlayer);
	void RemovePlayer(FString PlayerID);
	void UpdatePlayer(AHUBPlayerParam PlayerParam);  // PlayerID need to fit with an existing one
};
