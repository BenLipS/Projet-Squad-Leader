// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerHealthInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerHealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IPlayerHealthInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnPlayerHealthChanged(float newValue) = 0;

	UFUNCTION()
	virtual void OnPlayerMaxHealthChanged(float newValue) = 0;
};
