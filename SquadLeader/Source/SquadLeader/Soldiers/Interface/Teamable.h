// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ASoldierTeam;

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Teamable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UTeamable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API ITeamable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Team")
	virtual ASoldierTeam* GetTeam() = 0;

	UFUNCTION()
	virtual bool SetTeam(ASoldierTeam* _Team) = 0;
};
