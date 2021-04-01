// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OrderInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOrderInterface : public UInterface
{
	GENERATED_BODY()
};
class SQUADLEADER_API IOrderInterface
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void OnOrderInputPressed() = 0;

	UFUNCTION()
	virtual void OnOrderInputReleased() = 0;
};
