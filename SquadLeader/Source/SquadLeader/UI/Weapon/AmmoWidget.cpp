// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoWidget.h"
#include "../Interface/WeaponDelegateInterface.h"

#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/KismetMaterialLibrary.h"

void UAmmoWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (IWeaponDelegateInterface* WeaponDelegateInterface = Cast<IWeaponDelegateInterface>(ObjectIn); WeaponDelegateInterface)
	{
		WeaponDelegateInterface->AddWeaponDelegate(this);
	}
}

void UAmmoWidget::OnAmmoChanged(uint8 newValue)
{
	Ammo = newValue;
	if (Ammo > MaxAmmo)
	{
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialCollection, FName("MaxAmmo"), Ammo);
	}
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialCollection, FName("CurrentAmmo"), Ammo);
	TextAmmo->SetText(FText::FromString(FString::FromInt(Ammo)));
}

void UAmmoWidget::OnMaxAmmoChanged(uint8 newValue)
{
	MaxAmmo = newValue;
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialCollection, FName("MaxAmmo"), Ammo);
}