// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Mission.h"
#include "FormationMission.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UFormationMission : public UMission
{
	GENERATED_BODY()
	
public:
	UFormationMission();

	UFUNCTION()
	void InitFormation(int _id, MissionPriority _p, FVector _position);

	UFUNCTION()
		void UpdatePosition(const FVector _position) noexcept { m_position = _position; }

	UFUNCTION()
		FVector GetPosition() const noexcept { return m_position; }

protected:
	UPROPERTY()
	FVector m_position;
};
