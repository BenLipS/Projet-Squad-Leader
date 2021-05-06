// Fill out your copyright notice in the Description page of Project Settings.


#include "HitMarkerInterfaceDelegate.h"

void IHitMarkerInterfaceDelegate::OnDamageReceived(float damage, bool isHeadshot)
{
	for (auto MinimapInterface : HitMarkerDelegates)
	{
		MinimapInterface->OnDamageReceived(damage, isHeadshot);
	}
}

void IHitMarkerInterfaceDelegate::AddHitMarkerDelegate(IHitMarkerInterface* newDelegate)
{
	HitMarkerDelegates.AddUnique(newDelegate);
}

void IHitMarkerInterfaceDelegate::RemoveHitMarkerDelegate(IHitMarkerInterface* removeDelegate)
{
	HitMarkerDelegates.Remove(removeDelegate);
}
