// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SquadInterface.h"
#include "../../Soldiers/AIs/SoldierAI.h"
#include "SquadDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USquadDelegateInterface : public USquadInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API ISquadDelegateInterface : public ISquadInterface
{
	GENERATED_BODY()

protected:
	//TArray<TScriptInterface<IPlayerShieldInterface*>> Delegates;
	TArray<ISquadInterface*> SquadDelegates;

public:
	virtual void OnSquadChanged(const TArray<FSoldierAIData>& newValue);

	virtual void OnSquadHealthChanged(int index, float newHealth);

	virtual void OnSquadMaxHealthChanged(int index, float newMaxHealth);

	virtual void OnSquadShieldChanged(int index, float newShield);

	virtual void OnSquadMaxShieldChanged(int index, float newMaxShield);

	void AddSquadDelegate(ISquadInterface* newDelegate);
	void RemoveSquadDelegate(ISquadInterface* removeDelegate);
};
