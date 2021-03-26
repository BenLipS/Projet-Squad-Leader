// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Mission.h"
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
	UMissionList();

protected:
	TArray<UMission> m_missions;
	
};
