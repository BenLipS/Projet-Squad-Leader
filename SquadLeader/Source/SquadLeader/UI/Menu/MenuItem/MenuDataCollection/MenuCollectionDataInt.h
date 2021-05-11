// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCollectionDataItem.h"
#include "MenuCollectionDataInt.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuCollectionDataInt : public UMenuCollectionDataItem
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int DataValue = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int MinValue = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int MaxValue = 100;

public:
	UPROPERTY(BlueprintAssignable, Category = "Appearance|Event")
		FOnValueChangedEvent OnValueChangedEvent;
	DECLARE_EVENT(UMenuCollectionDataItem, FNativeOnValueChangedEvent);
	FNativeOnValueChangedEvent OnNativeValueChangedEvent;

public:
	UFUNCTION(BlueprintCallable)
		virtual void SetDataValue(int newValue);

	UFUNCTION(BlueprintCallable)
		int GetDataValue();

	UFUNCTION(BlueprintCallable)
		virtual void SetMinValue(int newValue);

	UFUNCTION(BlueprintCallable)
		int GetMinValue();

	UFUNCTION(BlueprintCallable)
		virtual void SetMaxValue(int newValue);

	UFUNCTION(BlueprintCallable)
		int GetMaxValue();
};
