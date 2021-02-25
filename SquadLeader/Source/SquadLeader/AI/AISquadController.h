// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGeneralController.h"
#include "AISquadController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AAISquadController : public AAIGeneralController
{
	GENERATED_BODY()
	
/////////// Respawn
public:
	virtual FVector GetRespawnPoint() override;
};
