// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../AIBasicController.h"
#include "HeavyAIBasicController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AHeavyAIBasicController : public AAIBasicController
{
	GENERATED_BODY()

	void CheckIfRegenShield() override;

public:
	AHeavyAIBasicController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void HomeTick(float DeltaSeconds) override;

	virtual void setup_BehaviorTree() override;

	virtual SoldierClass GetClass() override { return SoldierClass::HEAVY; }
};
