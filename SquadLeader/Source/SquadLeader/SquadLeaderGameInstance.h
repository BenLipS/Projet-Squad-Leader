#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AI/AIBasicManager.h"
#include "AI/AISquadManager.h"
#include "AI/AIBasicController.h"
#include "AI/InfluenceMap/GrdiSetUp.h"
#include "SquadLeaderGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API USquadLeaderGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAIBasicManager> AIBasicManagerClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AGrdiSetUp> InfluenceMapClass;

public:

	UPROPERTY()
	AAIBasicManager* AIBasicManagerTeam1;

	UPROPERTY()
	AAIBasicManager* AIBasicManagerTeam2;

	UPROPERTY()
	AGrdiSetUp* InfluenceMap;

	UPROPERTY()
	TArray<AAISquadManager*> ListAISquadManagers;

	USquadLeaderGameInstance();

	UFUNCTION()
	void InitAIManagers();

	UFUNCTION()
	void InitInfluenceMap();
	
	/*
	* For AI placed via drag and drop
	*/
	UFUNCTION()
	void AddAIBasicToManager(AAIBasicController* AIBasic);

	UFUNCTION()
	TArray<AAISquadManager*> GetSquadManagers() { return ListAISquadManagers; }

};
