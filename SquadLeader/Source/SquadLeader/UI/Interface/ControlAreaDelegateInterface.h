// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ControlAreaInterface.h"

#include "ControlAreaDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UControlAreaDelegateInterface : public UControlAreaInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IControlAreaDelegateInterface : public IControlAreaInterface
{
	GENERATED_BODY()

private:
	TArray<IControlAreaInterface*> ControlAreaDelegates;

public:
	void AddControlAreaDelegate(IControlAreaInterface* newDelegate);
	void RemoveControlAreaDelegate(IControlAreaInterface* removeDelegate);

	virtual void OnControlAreaInit(unsigned int nbArea) override;
	virtual void OnAreaCaptureChanged(unsigned int index, int owner, int capturer, float capturePercent) override;
};
