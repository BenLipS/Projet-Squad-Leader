// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureMission.h"
#include "../../ControlArea/ControlArea.h"

UCaptureMission::UCaptureMission() : m_controlArea{ nullptr } {
}

void UCaptureMission::Init(int _id, MissionPriority _p, AControlArea* _controlArea) {
	Super::Init(_id, _p);
	m_controlArea = _controlArea;
}