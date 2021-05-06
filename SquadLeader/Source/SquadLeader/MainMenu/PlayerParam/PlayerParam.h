// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
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

};
