// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SquadLeader/SquadLeader.h"

#include "AbilityCooldownInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAbilityCooldownInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IAbilityCooldownInterface
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void OnAbilityCooldownTriggered(float Timer, ESoldierAbilityInputID Key) = 0;

	UFUNCTION()
	virtual void AddAbilityID(ESoldierAbilityInputID Key, FString KeyText) = 0;
};
