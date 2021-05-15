// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCollectionDataItem.h"
#include "MenuCollectionDataText.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueStringChangedEvent, const FString&, newValue);

UCLASS()
class SQUADLEADER_API UMenuCollectionDataText : public UMenuCollectionDataItem
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString DataValue = "Default Text";

public:
	UPROPERTY(BlueprintAssignable, Category = "Appearance|Event")
	FOnValueStringChangedEvent OnValueChangedEvent;
	DECLARE_EVENT(UMenuCollectionDataText, FNativeOnValueStringChangedEvent);
	FNativeOnValueStringChangedEvent OnNativeValueChangedEvent;

public:
	UFUNCTION(BlueprintCallable)
	virtual bool SetDataValue(const FString& newValue, bool fireEvent = true);

	UFUNCTION(BlueprintCallable)
	FString GetDataValue();
};
