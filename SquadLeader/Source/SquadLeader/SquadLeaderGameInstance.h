#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerData/PlayerData.h"
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
	
public:
	void HttpCallPing(FString BaseAdress);
	
protected:
	void OnResponseReceivedPing(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
