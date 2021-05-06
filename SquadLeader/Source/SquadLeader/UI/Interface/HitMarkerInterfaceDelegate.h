// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitMarkerInterface.h"
#include "HitMarkerInterfaceDelegate.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitMarkerInterfaceDelegate : public UHitMarkerInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IHitMarkerInterfaceDelegate : public IHitMarkerInterface
{
	GENERATED_BODY()
private:
	TArray<IHitMarkerInterface*> HitMarkerDelegates;

public:
	virtual void OnDamageReceived(float damage, bool isHeadshot);

	void AddHitMarkerDelegate(IHitMarkerInterface* newDelegate);
	void RemoveHitMarkerDelegate(IHitMarkerInterface* removeDelegate);
};
