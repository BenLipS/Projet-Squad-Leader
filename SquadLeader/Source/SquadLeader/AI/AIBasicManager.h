// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIBasicController.h"
#include "AIBasicManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API AAIBasicManager : public AInfo
{
	GENERATED_BODY()

public:
	AAIBasicManager();

	UFUNCTION()
	void Init(TSubclassOf<ASoldierTeam> _Team, UObject* WorldContextObject);

	UPROPERTY()
	TArray<AAIBasicController*> AIBasicList;

	UPROPERTY()
	TSubclassOf<ASoldierTeam> Team;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ASoldierAI> ClassAI;
private:


};
