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
	const int PlayerNum() { return PlayersInfo.Num(); }

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(Client, reliable)
	void ClientAskArrayReplication();
	void ClientAskArrayReplication_Implementation();
	UFUNCTION(Server, reliable)
	void ServerAskArrayReplication();
	void ServerAskArrayReplication_Implementation();

	UFUNCTION(NetMulticast, reliable)
	void ClientSyncHUBParamArray(const TArray<AHUBPlayerParam*>& ServerPlayersInfo);
	void ClientSyncHUBParamArray_Implementation(const TArray<AHUBPlayerParam*>& ServerPlayersInfo);

public:
	UFUNCTION(Client, unreliable)
	void ClientRefreshPlayerInfo();
	void ClientRefreshPlayerInfo_Implementation();
	
	TMap<FString, FString> GetInfoAsStringPair();

	
	UFUNCTION(NetMulticast, reliable)
	void MulticastSetNewArrival(AHUBPlayerParam* NewPlayer);
	void MulticastSetNewArrival_Implementation(AHUBPlayerParam* NewPlayer);
	UFUNCTION(Server, reliable)
	void ServerSetNewArrival(AHUBPlayerParam* NewPlayer);
	void ServerSetNewArrival_Implementation(AHUBPlayerParam* NewPlayer);
	
	UFUNCTION(NetMulticast, reliable)
	void MulticastRemovePlayer(const FString& PlayerID);
	void MulticastRemovePlayer_Implementation(const FString& PlayerID);

	UFUNCTION(NetMulticast, reliable)
	void MulticastUpdatePlayer(AHUBPlayerParam* PlayerParam);  // PlayerID need to fit with an existing one
	void MulticastUpdatePlayer_Implementation(AHUBPlayerParam* PlayerParam);

protected:
	FTimerHandle timerTestReadyness;
	bool LastTestWasReady = false;
	void TestReadyness();
};
