// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionList.h"
#include "../AIGeneralController.h"


auto FMissionAction::operator()(UCaptureMission* _mission)const
{
	//GEngine->AddOnScreenDebugMessage(40, 10.f, FColor::Yellow, TEXT("Hello ! Mission de type UCaptureMission"));
	_mission->SetState(MissionState::eRUNNING);
	m_ai_controller->SetControlAreaBB(_mission->GetControlArea());
	m_ai_controller->BecomeUnavailable();
}
auto FMissionAction::operator()(UDefendMission* _mission)const
{
	//GEngine->AddOnScreenDebugMessage(40, 10.f, FColor::Yellow, TEXT("Mission de type UDefendMission"));
	_mission->SetState(MissionState::eRUNNING);
	m_ai_controller->BecomeUnavailable();
}
auto FMissionAction::operator()(UPatrolMission* _mission)const
{
	//GEngine->AddOnScreenDebugMessage(50, 10.f, FColor::Blue, TEXT("Mission de type UPatrolMission"));
	_mission->SetState(MissionState::eRUNNING);
	m_ai_controller->BecomeAvailable();
}
auto FMissionAction::operator()(UFormationMission* _mission)const
{
	//GEngine->AddOnScreenDebugMessage(50, 10.f, FColor::Blue, TEXT("Mission de type UFormationMission"));
	_mission->SetState(MissionState::eRUNNING);
	m_ai_controller->BecomeUnavailable();
}

auto FAIState::operator()(UCaptureMission* _mission) const {
	m_ai_controller->SetState(AIBasicState::Capturing);
}
auto FAIState::operator()(UPatrolMission* _mission) const {
	m_ai_controller->SetState(AIBasicState::Patroling);
}
auto FAIState::operator()(UDefendMission* _mission) const {
	m_ai_controller->SetState(AIBasicState::Defend);
}
auto FAIState::operator()(UFormationMission* _mission) const {
	m_ai_controller->SetState(AIBasicState::Formation);
}


UMissionList::UMissionList() : m_ai_controller{} {
}

void UMissionList::Init(AAIGeneralController* _ai_controller) {
	m_ai_controller = _ai_controller;
	m_action = FMissionAction{ m_ai_controller };
	m_stateChange = FAIState{ m_ai_controller };
	m_compare = FCompareMission{};
}

void UMissionList::Add(type_mission _mission) {
	if (m_missions.Num() <= 0)
		m_missions.Add(_mission);
	else {
		for (int i = 0; i != m_missions.Num(); ++i) {
			auto result = Visit(m_compare, _mission, m_missions[i]);
			if (result) {
				m_missions.Insert(_mission, i);
				m_index_current_mission = i;
				return;
			}
		}
		m_missions.Add(_mission);
	}

}

auto UMissionList::GetCurrentMission() const {
	return m_missions[m_index_current_mission];
}

void UMissionList::RunMission() {
	Visit(m_action, m_missions[m_index_current_mission]);
}

void UMissionList::EndMission() {
	if (m_missions.Num() > 1) {
		m_missions.RemoveAt(m_index_current_mission);
	}
	StateChange();
}

void UMissionList::StateChange(){
	if(m_missions.Num() > 0)
		Visit(m_stateChange, m_missions[m_index_current_mission]);
}

void UMissionList::Empty() {
	m_index_current_mission = 0;
	m_missions.Empty();
}