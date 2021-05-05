// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../AISquadController.h"
#include "HeavyAISquadController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AHeavyAISquadController : public AAISquadController
{
	GENERATED_BODY()

	void CheckIfRegenShield() override;

public:
	AHeavyAISquadController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void setup_BehaviorTree() override;

	virtual SoldierClass GetClass() override { return SoldierClass::HEAVY; }
};
