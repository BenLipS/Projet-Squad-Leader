// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionList.h"
#include "../AIGeneralController.h"


auto FMissionAction::operator()(UCaptureMission* _mission)const
{
	GEngine->AddOnScreenDebugMessage(40, 10.f, FColor::Yellow, TEXT("Hello ! Mission de type UCaptureMission"));
	_mission->SetState(MissionState::eRUNNING);
	m_ai_controller->SetControlAreaBB(_mission->GetControlArea());
}
auto FMissionAction::operator()(UDefendMission* _mission)const
{
	GEngine->AddOnScreenDebugMessage(40, 10.f, FColor::Yellow, TEXT("Mission de type UDefendMission"));
	_mission->SetState(MissionState::eRUNNING);
}
auto FMissionAction::operator()(UPatrolMission* _mission)const
{
	GEngine->AddOnScreenDebugMessage(50, 10.f, FColor::Blue, TEXT("Mission de type UPatrolMission"));
	_mission->SetState(MissionState::eRUNNING);
	m_ai_controller->SetObjectifLocation(m_ai_controller->GetPawn()->GetActorLocation());
}


UMissionList::UMissionList() : m_ai_controller{} {
}

void UMissionList::Init(AAIGeneralController* _ai_controller) {
	m_ai_controller = _ai_controller;
	m_action = FMissionAction{ m_ai_controller };
}

void UMissionList::Add(type_mission _mission) {
	if (m_missions.Num() <= 0)
		m_missions.Add(_mission);
	//for (int i = 0; i != m_missions.Num(); ++i) {

	//}
}

auto UMissionList::GetCurrentMission() const {
	return m_missions.Top();
}

void UMissionList::RunMission() {
	Visit(m_action, m_missions.Top());
}