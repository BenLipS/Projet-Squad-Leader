// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCollectionDataInt.h"

#include "Components/EditableTextBox.h"

#include "Types/SlateEnums.h"

#include "MenuCollectionDataTextBoxInt.generated.h"

UCLASS()
class SQUADLEADER_API UMenuCollectionDataTextBoxInt : public UMenuCollectionDataInt
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* TextBoxData;

protected:
	virtual void SynchronizeProperties() override;

public:
	virtual bool SetDataValue(int newValue, bool fireEvent = true) override;
	virtual bool SetMinValue(int newValue) override;
	virtual bool SetMaxValue(int newValue) override;
};
