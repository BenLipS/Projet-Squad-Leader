// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Interface/HitMarkerInterfaceDelegate.h"
#include "Interface/HitMarkerInterface.h"
#include "HitMarkerWidget.generated.h"

UCLASS()
class SQUADLEADER_API UHitMarkerWidget : public USL_UserWidget, public IHitMarkerInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitMarker")
	class UMaterialParameterCollection* MaterialCollection;

	/*How much each point of damage infliged affect the hitmarker*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitMarker")
	float DamageValue = 0.1f;

	/*How fast it take tfor a full HitMarker to disappear (base on a time of 1s)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitMarker")
	float PlaySpeed = 1.f;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HitMarkerAnimation;

public:
	void SetupDelegateToObject_Implementation(UObject* ObjectIn) override;
	
//-----IHitMarkerInterface-----
public:
	virtual void OnDamageReceived(float damage, bool isHeadshot) override;
};
