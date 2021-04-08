// Fill out your copyright notice in the Description page of Project Settings.


#include "FormationMission.h"

UFormationMission::UFormationMission() : m_position{ FVector{} } {}

void UFormationMission::InitFormation(int _id, MissionPriority _p, FVector _position) {
	Super::Init(_id, _p);
	m_position = _position;
	m_state = MissionState::eRUNNING;
	m_type = MissionType::Formation;
}