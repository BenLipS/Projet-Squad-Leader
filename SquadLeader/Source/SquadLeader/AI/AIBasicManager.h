// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIBasicController.h"
#include "AIBasicManager.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UAIBasicManager : public UObject
{
	GENERATED_BODY()

public:
	UAIBasicManager() = default;

	UFUNCTION()
	void Init(TSubclassOf<ASoldierTeam> _Team);

	UPROPERTY()
	TArray<AAIBasicController*> AIBasicList;

	UPROPERTY()
	TSubclassOf<ASoldierTeam> Team;

};
