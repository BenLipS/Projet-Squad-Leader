// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "WheelWidgetElement.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UWheelWidgetElement : public USL_UserWidget
{
	GENERATED_BODY()
	
public:
	virtual void OnitemSelected() {}

	//Return true if state has changed
	virtual void SetItemHovered(bool bIsHovered);
	bool IsItemHovered();

protected:
	bool bIsItemHovered = false;
};
