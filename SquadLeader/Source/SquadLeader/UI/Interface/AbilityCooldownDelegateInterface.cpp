// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityCooldownDelegateInterface.h"

// Add default functionality here for any IAbilityCooldownDelegateInterface functions that are not pure virtual.

void IAbilityCooldownDelegateInterface::OnAbilityCooldownTriggered(float Timer, ESoldierAbilityInputID Key)
{
	for (auto AbilityCooldownInterface : AbilityCooldownDelegates)
	{
		AbilityCooldownInterface->OnAbilityCooldownTriggered(Timer, Key);
	}
}

void IAbilityCooldownDelegateInterface::AddAbilityID(ESoldierAbilityInputID Key)
{
	for (auto AbilityCooldownInterface : AbilityCooldownDelegates)
	{
		AbilityCooldownInterface->AddAbilityID(Key);
	}
}

void IAbilityCooldownDelegateInterface::AddAbilityCooldownDelegate(IAbilityCooldownInterface* newDelegate)
{
	AbilityCooldownDelegates.AddUnique(newDelegate);
}

void IAbilityCooldownDelegateInterface::RemoveAbilityCooldownDelegate(IAbilityCooldownInterface* removeDelegate)
{
	AbilityCooldownDelegates.Remove(removeDelegate);
}
