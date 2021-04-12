// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../AISquadController.h"
#include "SupportAISquadController.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API ASupportAISquadController : public AAISquadController
{
	GENERATED_BODY()

	UFUNCTION()
	void CheckIfLaunchHeal();

	UFUNCTION()
	void CheckIfLaunchShield();

public:
	ASupportAISquadController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void setup_BehaviorTree() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal")
	float RatioBeforeHeal = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal")
	float SinglePlayerRatioBeforeHeal = 0.2f;
};
