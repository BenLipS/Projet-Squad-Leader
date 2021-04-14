// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolMission.h"

UPatrolMission::UPatrolMission(){}

void UPatrolMission::InitPatrolMission(int _id, MissionPriority _p) {
	Super::Init(_id, _p);
	m_type = MissionType::ePATROL;
}