// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ControlAreaInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UControlAreaInterface : public UInterface
{
	GENERATED_BODY()
};


class SQUADLEADER_API IControlAreaInterface
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void OnControlAreaInit(unsigned int nbArea) = 0;
	// Owner : -1 ennemy, 0 neutral, 1 ally
	UFUNCTION()
	virtual void OnAreaCaptureChanged(unsigned int index, int owner, int capturer, float capturePercent) = 0;
};
