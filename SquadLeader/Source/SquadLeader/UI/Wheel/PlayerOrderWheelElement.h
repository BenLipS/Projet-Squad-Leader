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
	friend class UPlayerOrderWheelWidget;
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Widget Wheel", BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	MissionType Order;
	
public:
	virtual void OnitemSelected() override;
};
