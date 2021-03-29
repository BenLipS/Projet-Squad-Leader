// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../AIBasicController.h"
#include "AssaultAIBasicController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AAssaultAIBasicController : public AAIBasicController
{
	GENERATED_BODY()
public:
	UFUNCTION(BluePrintCallable, Category = "Comportement")
		virtual void Tick(float DeltaSeconds) override;
};
