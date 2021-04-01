// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerShieldInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerShieldInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class SQUADLEADER_API IPlayerShieldInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
		virtual void OnPlayerShieldChanged(float newValue) = 0;

	UFUNCTION()
		virtual void OnPlayerMaxShieldChanged(float newValue) = 0;
};
