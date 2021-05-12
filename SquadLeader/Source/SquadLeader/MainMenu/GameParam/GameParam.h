// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/Info.h"
#include "Net/UnrealNetwork.h"
#include "GameParam.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AGameParam : public AInfo
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGameParam();
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;


	UPROPERTY(EditDefaultsOnly, Replicated)
		FString Name;
	UPROPERTY(EditDefaultsOnly, Replicated)
		int LevelTarget; //Not fixed by user
	UPROPERTY(EditDefaultsOnly, Replicated)
		int LevelRange; //Not fixed by user
	UPROPERTY(EditDefaultsOnly, Replicated)
		int NbAIBasicAssault;
	UPROPERTY(EditDefaultsOnly, Replicated)
		int NbAIBasicHeavy;
	UPROPERTY(EditDefaultsOnly, Replicated)
		int LevelAIBasic;
	UPROPERTY(EditDefaultsOnly, Replicated)
		int StartingNbAISquad;
	UPROPERTY(EditDefaultsOnly, Replicated)
		int LevelAISquad;
	UPROPERTY(EditDefaultsOnly, Replicated)
		int NbTickets;
	UPROPERTY(EditDefaultsOnly, Replicated)
		int Weather; //Not fixed by user
	UPROPERTY(EditDefaultsOnly, Replicated)
		int RespawnDuration;
	UPROPERTY(EditDefaultsOnly, Replicated)
		bool FriendOnly; //Not fixed by user

	void RandomiseParam(AGameParam* MinConfig, AGameParam* MaxConfig);

	void GameParamCopy(AGameParam* TargetParam);

	UFUNCTION()
	TMap<FString, int> GetIntParams();

	UFUNCTION()
	TMap<FString, FString> GetStringParams();
};
