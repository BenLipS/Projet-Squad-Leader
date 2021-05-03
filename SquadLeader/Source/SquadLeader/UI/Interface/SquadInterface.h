// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SquadLeader/AI/Mission.h"

#include "SquadInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USquadInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API ISquadInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnSquadChanged(const TArray<FSoldierAIData>& newValue) = 0;

	UFUNCTION()
	virtual void OnSquadHealthChanged(int index, float newHealth) = 0;

	UFUNCTION()
	virtual void OnSquadMaxHealthChanged(int index, float newMaxHealth) = 0;

	UFUNCTION()
	virtual void OnSquadShieldChanged(int index, float newShield) = 0;

	UFUNCTION()
	virtual void OnSquadMaxShieldChanged(int index, float newMaxShield) = 0;

	UFUNCTION()
	virtual void OnSquadMemberMissionChanged(int index, AIBasicState newMission) = 0;
};
