// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCollectionDataItem.h"
#include "MenuCollectionDataBool.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueBoolChangedEvent, bool, newValue);

UCLASS()
class SQUADLEADER_API UMenuCollectionDataBool : public UMenuCollectionDataItem
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool DataValue = false;

public:
	UPROPERTY(BlueprintAssignable, Category = "Appearance|Event")
	FOnValueBoolChangedEvent OnValueChangedEvent;
	DECLARE_EVENT(UMenuCollectionDataBool, FNativeOnValueBoolChangedEvent);
	FNativeOnValueBoolChangedEvent OnNativeValueChangedEvent;

public:
	UFUNCTION(BlueprintCallable)
	virtual bool SetDataValue(bool newValue);

	UFUNCTION(BlueprintCallable)
	bool GetDataValue();
};
