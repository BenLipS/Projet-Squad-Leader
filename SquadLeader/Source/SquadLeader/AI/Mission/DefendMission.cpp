// Fill out your copyright notice in the Description page of Project Settings.


#include "DefendMission.h"
#include "../../ControlArea/ControlArea.h"

UDefendMission::UDefendMission() : m_controlArea{ nullptr } {
}

void UDefendMission::InitDefendMission(int _id, MissionPriority _p, AControlArea* _controlArea) {
	Super::Init(_id, _p);
	m_controlArea = _controlArea;
}