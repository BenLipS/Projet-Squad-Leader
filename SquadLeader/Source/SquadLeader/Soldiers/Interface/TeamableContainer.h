// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ASoldierTeam;

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Teamable.h"
#include "TeamableContainer.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTeamableContainer : public UTeamable
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API ITeamableContainer : public ITeamable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
private:
	TSubclassOf<ASoldierTeam> Team = nullptr;

public:
		virtual TSubclassOf<ASoldierTeam> GetTeam() override;

		virtual bool SetTeam(TSubclassOf<ASoldierTeam> _Team) override;
};
