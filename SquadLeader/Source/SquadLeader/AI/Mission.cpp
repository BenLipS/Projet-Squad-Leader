// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission.h"

UMission::UMission() : m_id{ -1 }, m_priority{ MissionPriority::eBASIC }, m_state{MissionState::eNOT_STARTED_YET}{
}

UMission::UMission(int _id, MissionPriority _p) : m_id{ _id }, m_priority{ _p } {}

void UMission::Init(int _id, MissionPriority _p) {
	m_id = _id;
	m_priority = _p;
	m_state = MissionState::eNOT_STARTED_YET;
}