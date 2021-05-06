// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "../../Soldiers/AIs/SoldierAI.h"
#include "../../Soldiers/Players/SoldierPlayer.h"
#include "PlayerParam.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API APlayerParam : public AInfo
{
	GENERATED_BODY()
public:
	int TeamID = 0;  // 0 for null
	TSubclassOf<ASoldierPlayer> PlayerClass;
	TSubclassOf<ASoldierAI> ClassAI1;
	TSubclassOf<ASoldierAI> ClassAI2;
	TSubclassOf<ASoldierAI> ClassAI3;
	TSubclassOf<ASoldierAI> ClassAI4;
	TSubclassOf<ASoldierAI> ClassAI5;
};
