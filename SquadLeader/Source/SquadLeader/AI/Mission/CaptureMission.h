// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Mission.h"
#include "CaptureMission.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UCaptureMission : public UMission
{
	GENERATED_BODY()
	
public:
	UCaptureMission();

protected:
	UPROPERTY()
		class AControlArea* m_controlArea;
public:

	void InitCaptureMission(int _id, MissionPriority _p, AControlArea* _controlArea);

	AControlArea* GetControlArea() const noexcept { return m_controlArea; }
private:
	void SetControlArea(AControlArea* _ca) noexcept { m_controlArea = _ca; }
};
