// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Mission.h"
#include "DefendMission.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UDefendMission : public UMission
{
	GENERATED_BODY()
	
public:
	UDefendMission();

protected:
	UPROPERTY()
		class AControlArea* m_controlArea;
public:
	void InitDefendMission(int _id, MissionPriority _p, AControlArea* _controlArea);
private:
	void SetControlArea(AControlArea* _ca) noexcept { m_controlArea = _ca; }
};
