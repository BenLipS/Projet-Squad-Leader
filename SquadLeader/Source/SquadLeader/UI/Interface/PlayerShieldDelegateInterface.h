// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerShieldInterface.h"
#include "PlayerShieldDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerShieldDelegateInterface : public UPlayerShieldInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class SQUADLEADER_API IPlayerShieldDelegateInterface : public IPlayerShieldInterface
{
	GENERATED_BODY()

private:
	TArray<IPlayerShieldInterface*> PlayerShieldDelegates;

public:
	virtual void OnPlayerShieldChanged(float newValue) override;

	virtual void OnPlayerMaxShieldChanged(float newValue) override;

	void AddPlayerShieldDelegate(IPlayerShieldInterface* newDelegate);
	void RemovePlayerShieldDelegate(IPlayerShieldInterface* removeDelegate);
};