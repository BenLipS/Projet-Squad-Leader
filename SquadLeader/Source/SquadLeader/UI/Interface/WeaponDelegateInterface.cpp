// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDelegateInterface.h"
#include "WeaponDelegateInterface.h"

void IWeaponDelegateInterface::OnAmmoChanged(uint8 newValue)
{
	for (auto WeaponInterface : WeaponDelegates)
	{
		WeaponInterface->OnAmmoChanged(newValue);
	}
}

void IWeaponDelegateInterface::OnMaxAmmoChanged(uint8 newValue)
{
	for (auto WeaponInterface : WeaponDelegates)
	{
		WeaponInterface->OnMaxAmmoChanged(newValue);
	}
}

void IWeaponDelegateInterface::AddWeaponDelegate(IWeaponInterface* newDelegate)
{
	WeaponDelegates.AddUnique(newDelegate);
}

void IWeaponDelegateInterface::RemoveWeaponDelegate(IWeaponInterface* removeDelegate)
{
	WeaponDelegates.Remove(removeDelegate);
}
