// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrderInterface.h"
#include "OrderDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOrderDelegateInterface : public UOrderInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IOrderDelegateInterface : public IOrderInterface
{
	GENERATED_BODY()
protected:
	TArray<IOrderInterface*> OrderDelegates;
public:
	virtual void OnOrderInputPressed();

	virtual void OnOrderInputReleased();


	void AddOrderDelegate(IOrderInterface* newDelegate);
	void RemoveOrderDelegate(IOrderInterface* removeDelegate);
};
