// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CaptureMission.h"
#include "DefendMission.h"
#include "PatrolMission.h"
#include "MissionList.generated.h"


/**
 * This class represent the functionnality of a Mission Manager List
 * Each AI will possess one
 */
UCLASS()
class SQUADLEADER_API UMissionList : public UObject
{
	GENERATED_BODY()

public:
	using type_mission = TVariant<UCaptureMission*, UDefendMission*, UPatrolMission*>;
	using array_mission_type = TArray<type_mission>;

	UMissionList();

protected:
	array_mission_type m_missions;
};
