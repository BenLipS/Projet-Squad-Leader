// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../AISquadController.h"
#include "AssaultAISquadController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AAssaultAISquadController : public AAISquadController
{
	GENERATED_BODY()
public:
	AAssaultAISquadController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void setup_BehaviorTree() override;

	UPROPERTY(BlueprintReadWrite, Category = "Grenade")
		int NUmberOfEnemyToLaunchAGrenade = 2;

	UPROPERTY(BlueprintReadWrite, Category = "OverHeat")
		int NUmberOfEnemyToOverHeat = 2;

	virtual SoldierClass GetClass() override { return SoldierClass::ASSAULT; }
};
