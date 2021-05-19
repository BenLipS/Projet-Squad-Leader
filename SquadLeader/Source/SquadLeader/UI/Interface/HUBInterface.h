// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInterfaceDelegate.h"
#include "HUBInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHUBInterface : public UBaseInterfaceDelegate
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IHUBInterface : public IBaseInterfaceDelegate
{
	GENERATED_BODY()
private:
	TArray<IHUBInterface*> HUBDelegates;

public:
	UFUNCTION()
	virtual void ClearPlayerList1();
	UFUNCTION()
	virtual void ClearPlayerList2();
	UFUNCTION()
	virtual void AddPlayerList1(TMap<FString, FString> statsIn);
	UFUNCTION()
	virtual void AddPlayerList2(TMap<FString, FString> statsIn);

	void AddHUBDelegate(IHUBInterface* newDelegate);
	void RemoveHUBsDelegate(IHUBInterface* removeDelegate);
};
