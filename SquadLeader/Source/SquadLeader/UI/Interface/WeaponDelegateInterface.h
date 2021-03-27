// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "WeaponDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponDelegateInterface : public UWeaponInterface
{
	GENERATED_BODY()
};

class SQUADLEADER_API IWeaponDelegateInterface : public IWeaponInterface
{
	GENERATED_BODY()

protected:
	TArray<IWeaponInterface*> WeaponDelegates;

public:
	virtual void OnAmmoChanged(uint8 newValue) override;

	virtual void OnMaxAmmoChanged(uint8 newValue) override;

	void AddWeaponDelegate(IWeaponInterface* newDelegate);
	void RemoveWeaponDelegate(IWeaponInterface* removeDelegate);
};
