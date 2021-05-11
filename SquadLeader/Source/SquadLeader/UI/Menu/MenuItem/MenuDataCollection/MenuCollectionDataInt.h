// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCollectionDataItem.h"
#include "MenuCollectionDataInt.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueIntChangedEvent, int, newValue);

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
	FOnValueIntChangedEvent OnValueChangedEvent;
	DECLARE_EVENT(UMenuCollectionDataInt, FNativeOnValueIntChangedEvent);
	FNativeOnValueIntChangedEvent OnNativeValueChangedEvent;

public:
	UFUNCTION(BlueprintCallable)
		virtual bool SetDataValue(int newValue, bool fireEvent = true);

	UFUNCTION(BlueprintCallable)
		int GetDataValue();

	UFUNCTION(BlueprintCallable)
		virtual bool SetMinValue(int newValue);

	UFUNCTION(BlueprintCallable)
		int GetMinValue();

	UFUNCTION(BlueprintCallable)
		virtual bool SetMaxValue(int newValue);

	UFUNCTION(BlueprintCallable)
		int GetMaxValue();
};
