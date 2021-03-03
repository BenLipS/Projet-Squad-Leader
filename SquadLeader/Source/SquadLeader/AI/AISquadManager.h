// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Soldiers/Players/SoldierPlayer.h"
#include "AISquadController.h"
#include "../Soldiers/AIs/SoldierAI.h"
#include "AISquadManager.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API UAISquadManager : public UObject
{
	GENERATED_BODY()

public:
	//UAISquadManager() = default;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ASoldierAI> ClassAI;

	UFUNCTION()
	void Init(TSubclassOf<ASoldierTeam> _Team, ASoldierPlayer* _Player, UObject* WorldContextObject);
	
	UPROPERTY()
	TSubclassOf<ASoldierTeam> Team;

	UPROPERTY()
	ASoldierPlayer* Leader;

	UPROPERTY()
	TArray<AAISquadController*> AISquadList;
};
