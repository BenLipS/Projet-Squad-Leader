// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SL_HUBGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASL_HUBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void Logout(AController* Exiting) override;

	UPROPERTY(BlueprintReadOnly, Category = "GameData")
		int NbMaxPlayer = 6;

private:
	void TeleportAllPlayersToGame();  // non-seamless travel

public:
	void SetNewArrival(class AHUBPlayerParam* NewPlayer);
	void UpdatePlayer(class AHUBPlayerParam* PlayerParam);
	void RemovePlayer(const FString& PlayerID);

protected:
	TArray<AHUBPlayerParam*> PlayersInfo;

	FTimerHandle timerTestReadyness;
	bool LastTestWasReady = false;
	void TestReadyness();
	void RefreshPlayerInfo();
};