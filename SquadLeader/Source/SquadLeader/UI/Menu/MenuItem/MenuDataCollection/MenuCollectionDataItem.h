// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../SL_UserWidget.h"
#include "MenuCollectionDataItem.generated.h"


 DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueChangedEvent, int, newValue);

UCLASS()
class SQUADLEADER_API UMenuCollectionDataItem : public USL_UserWidget
{
	GENERATED_BODY()

};