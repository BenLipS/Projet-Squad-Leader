// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolControlAreaMission.h"
#include "../../ControlArea/ControlArea.h"

UPatrolControlAreaMission::UPatrolControlAreaMission() : ControlArea{ nullptr }{}

void UPatrolControlAreaMission::InitPatrolControlAreaMission(const uint32 Id, const MissionPriority Priority, AControlArea* _ControlArea) {
	Super::Init(Id, Priority);
	ControlArea = _ControlArea;
	m_type = MissionType::ePATROL;
}