// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../AIBasicController.h"
#include "AssaultAIBasicController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AAssaultAIBasicController : public AAIBasicController
{
	GENERATED_BODY()
public:
	AAssaultAIBasicController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void setup_BehaviorTree() override;

	UPROPERTY(BlueprintReadWrite, Category = "Grenade")
	int NUmberOfEnemyToLaunchAGrenade = 2;

	UPROPERTY(BlueprintReadWrite, Category = "OverHeat")
		int NUmberOfEnemyToOverHeat = 2;
};
