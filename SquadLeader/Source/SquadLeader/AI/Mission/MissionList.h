// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CaptureMission.h"
#include "DefendMission.h"
#include "PatrolMission.h"
#include "Misc/TVariant.h"
#include "MissionList.generated.h"

/**
 * This class represent the functionnality of a Mission Manager List
 * Each AI will possess one
 */


USTRUCT()
struct SQUADLEADER_API FMissionAction {
	GENERATED_USTRUCT_BODY()

	class AAIGeneralController* m_ai_controller;

	FMissionAction() = default;
	FMissionAction(AAIGeneralController* _ai_controller) { m_ai_controller = _ai_controller; }

	auto operator()(UCaptureMission* _mission)const;
	auto operator()(UDefendMission* _mission)const;
	auto operator()(UPatrolMission* _mission)const;
};




UCLASS()
class SQUADLEADER_API UMissionList : public UObject
{
	GENERATED_BODY()

public:
	using type_mission = TVariant<UCaptureMission*, UDefendMission*, UPatrolMission*>;
	using array_mission_type = TArray<type_mission>;

	UMissionList();

protected:
	UPROPERTY()
	class AAIGeneralController* m_ai_controller;

	array_mission_type m_missions;

	FMissionAction m_action;

	UPROPERTY()
	int m_id_current_mission = -1;


public:
	/*
	* Initialize the Object
	*/
	UFUNCTION()
		void Init(AAIGeneralController* _ai_controller);

	/*
	* Add the mission at the good place
	* After this the Array is still sorted
	*/
	void Add(type_mission _mission);

	/*
	* Return the current mission
	*/
	auto GetCurrentMission() const;

	UFUNCTION()
		void RunMission();
};
