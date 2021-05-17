// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BaseInterfaceDelegate.h"

#include "RespawnInterface.generated.h"

UINTERFACE(MinimalAPI)
class URespawnInterface : public UBaseInterfaceDelegate
{
	GENERATED_BODY()
};

class SQUADLEADER_API IRespawnInterface : public IBaseInterfaceDelegate
{
	GENERATED_BODY()
private:
	TArray<IRespawnInterface*> RespawnDelegates;

public:
	UFUNCTION()
	virtual void OnRespawnScreenActivated();

	void AddRespawnDelegate(IRespawnInterface* newDelegate);
	void RemoveRespawnDelegate(IRespawnInterface* removeDelegate);
};
