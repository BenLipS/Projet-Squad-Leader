// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerHealthInterface.h"
#include "PlayerHealthDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerHealthDelegateInterface : public UPlayerHealthInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IPlayerHealthDelegateInterface : public IPlayerHealthInterface
{
	GENERATED_BODY()

protected:
	//TArray<TScriptInterface<IPlayerHealthInterface*>> Delegates;
	TArray<IPlayerHealthInterface*> PlayerHealthDelegates;

public:
	virtual void OnPlayerHealthChanged(float newValue) override;

	virtual void OnPlayerMaxHealthChanged(float newValue) override;

	void AddPlayerHealthDelegate(IPlayerHealthInterface* newDelegate);
	void RemovePlayerHealthDelegate(IPlayerHealthInterface* removeDelegate);
};