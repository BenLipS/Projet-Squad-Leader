// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadDelegateInterface.h"

void ISquadDelegateInterface::OnSquadChanged(const TArray<FSoldierAIData>& newValue)
{
	for (auto SquadInterface : SquadDelegates)
	{
		SquadInterface->OnSquadChanged(newValue);
	}
}

void ISquadDelegateInterface::OnSquadHealthChanged(int index, float newHealth)
{
	for (auto SquadInterface : SquadDelegates)
	{
		SquadInterface->OnSquadHealthChanged(index, newHealth);
	}
}

void ISquadDelegateInterface::OnSquadMaxHealthChanged(int index, float newMaxHealth)
{
	for (auto SquadInterface : SquadDelegates)
	{
		SquadInterface->OnSquadMaxHealthChanged(index, newMaxHealth);
	}
}

void ISquadDelegateInterface::OnSquadShieldChanged(int index, float newShield)
{
	for (auto SquadInterface : SquadDelegates)
	{
		SquadInterface->OnSquadShieldChanged(index, newShield);
	}
}

void ISquadDelegateInterface::OnSquadMaxShieldChanged(int index, float newMaxShield)
{
	for (auto SquadInterface : SquadDelegates)
	{
		SquadInterface->OnSquadMaxShieldChanged(index, newMaxShield);
	}
}

void ISquadDelegateInterface::OnSquadMemberMissionChanged(int index, AIBasicState newMission)
{
	for (auto SquadInterface : SquadDelegates)
	{
		SquadInterface->OnSquadMemberMissionChanged(index, newMission);
	}
}

void ISquadDelegateInterface::AddSquadDelegate(ISquadInterface* newDelegate)
{
	SquadDelegates.AddUnique(newDelegate);
}

void ISquadDelegateInterface::RemoveSquadDelegate(ISquadInterface* removeDelegate)
{
	SquadDelegates.Remove(removeDelegate);
}
