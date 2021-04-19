// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityCooldownInterface.h"
#include "AbilityCooldownDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAbilityCooldownDelegateInterface : public UAbilityCooldownInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IAbilityCooldownDelegateInterface : public IAbilityCooldownInterface
{
	GENERATED_BODY()

private:
	TArray<IAbilityCooldownInterface*> AbilityCooldownDelegates;
public:
	virtual void OnAbilityCooldownTriggered(float Timer, ESoldierAbilityInputID Key) override;
	virtual void AddAbilityID(ESoldierAbilityInputID Key) override;

	void AddAbilityCooldownDelegate(IAbilityCooldownInterface* newDelegate);
	void RemoveAbilityCooldownDelegate(IAbilityCooldownInterface* removeDelegate);
};
