// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Mission.generated.h"

/**
 * Mission that will be send to an AI
 */

UENUM()
enum class MissionType{ None, Defence, Attack, MoveTo };

UCLASS()
class SQUADLEADER_API UMission : public UObject
{
	GENERATED_BODY()

public:
	UMission() = default;

	UPROPERTY()
	MissionType Type;

	FVector Location;
};
