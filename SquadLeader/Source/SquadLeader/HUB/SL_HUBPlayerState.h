#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HUBPlayerParam.h"
#include "SL_HUBPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASL_HUBPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void ChangeTeam();
	UFUNCTION(BlueprintCallable)
		void ChangeReadyState();

	UFUNCTION(Client, Reliable)
	void ClientRemoveHUBPlayerParam();
	void ClientRemoveHUBPlayerParam_Implementation();

protected:
	AHUBPlayerParam* LocalHUBPlayerParam;
	TArray<AHUBPlayerParam*> PlayerParamList;

protected:
	// function to call server
	UFUNCTION(Server, reliable)
	void ServerSetNewArrival(AHUBPlayerParam* NewPlayer);
	void ServerSetNewArrival_Implementation(AHUBPlayerParam* NewPlayer);

	UFUNCTION(Server, reliable)
	void ServerUpdatePlayer(AHUBPlayerParam* PlayerParam);  // PlayerID need to fit with an existing one
	void ServerUpdatePlayer_Implementation(AHUBPlayerParam* PlayerParam);

	UFUNCTION(Server, reliable)
	void ServerRemovePlayerParam(const FString& RemovePlayerID);
	void ServerRemovePlayerParam_Implementation(const FString& RemovePlayerID);

public:
	// function called by the server
	UFUNCTION(Client, reliable)
	void ClientRefreshPlayerInfo(const TArray<AHUBPlayerParam*>& PlayerParam);
	void ClientRefreshPlayerInfo_Implementation(const TArray<AHUBPlayerParam*>& PlayerParam);
protected:
	TMap<FString, FString> GetInfoAsStringPair();
};
