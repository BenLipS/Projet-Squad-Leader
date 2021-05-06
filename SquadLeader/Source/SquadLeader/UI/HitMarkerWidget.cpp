// Fill out your copyright notice in the Description page of Project Settings.


#include "HitMarkerWidget.h"

#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/KismetMaterialLibrary.h"

void UHitMarkerWidget::OnDamageReceived(float damage, bool isHeadshot)
{
	if (PlaySpeed > 0.f)
	{
		float ForceHitMarker = UKismetMaterialLibrary::GetScalarParameterValue(GetWorld(), MaterialCollection, FName("Force"));
		ForceHitMarker += damage * DamageValue;
		ForceHitMarker = FMath::Clamp(ForceHitMarker, 0.f, 1.f);

		PlayAnimation(HitMarkerAnimation, 1 - ForceHitMarker, 1, EUMGSequencePlayMode::Forward, PlaySpeed);
	}
}