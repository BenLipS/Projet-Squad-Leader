#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainMenu/PlayerData/PlayerData.h"
#include "MainMenu/GameParam/GameParam.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "SquadLeaderGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class SQUADLEADER_API USquadLeaderGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USquadLeaderGameInstance();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UserDataLocation")
		FString BaseServerDataAdress;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UserDataLocation")
		FString UserDataFilename;
	
protected:
	void OnStart();
	FHttpModule* Http;

	PlayerData UserData;

	FString AuthToken;
	
public:
	UFUNCTION(BlueprintCallable)
		void LaunchGame();
	UFUNCTION(BlueprintCallable)
		void SetGameParamToDefault();
	UFUNCTION(BlueprintCallable)
		void SetGameParamToRandom();
	UFUNCTION(BlueprintCallable)
		void JoinGame(FString IPAdress);
	UFUNCTION(BlueprintCallable)
		void ProfileInfo(class ASL_HUD* HUD);


private:
	void HttpCallPing(FString BaseAdress);
	void HttpCallCreateUser(FString BaseAdress);
	void HttpCallConnectUser(FString BaseAdress);
	void HttpCallSendSyncData(FString BaseAdress);
	void HttpCallReceiveSyncData(FString BaseAdress);
	
private:
	void OnResponseReceivedPing(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseReceivedCreateUser(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseReceivedConnectUser(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseReceivedSendSync(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseReceivedReceiveSync(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <UGameParam> GameParam;
};
