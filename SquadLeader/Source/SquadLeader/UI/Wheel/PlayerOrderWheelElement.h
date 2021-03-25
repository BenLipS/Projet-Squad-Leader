// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheelWidgetElement.h"
#include "../../AI/Mission.h"
#include "PlayerOrderWheelElement.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UPlayerOrderWheelElement : public UWheelWidgetElement
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Wheel order")
	MissionType Order;
	
public:
	virtual void OnitemSelected() override;
};
