// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Mission.h"
#include "PatrolMission.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UPatrolMission : public UMission
{
	GENERATED_BODY()

public:
	UPatrolMission();

	UFUNCTION()
	void InitPatrolMission(int _id, MissionPriority _p);
};
