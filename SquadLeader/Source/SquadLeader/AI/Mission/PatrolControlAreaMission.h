// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Mission.h"
#include "PatrolControlAreaMission.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UPatrolControlAreaMission : public UMission
{
	GENERATED_BODY()
	
public:
	UPatrolControlAreaMission();

protected:
	class AControlArea* ControlArea;

public:
	void InitPatrolControlAreaMission(const uint32 Id, const MissionPriority Priority, AControlArea* ControlArea);
	AControlArea* GetControlArea() const noexcept { return ControlArea; }
	void SetControlArea(AControlArea* CA) noexcept { ControlArea = CA; }
};
