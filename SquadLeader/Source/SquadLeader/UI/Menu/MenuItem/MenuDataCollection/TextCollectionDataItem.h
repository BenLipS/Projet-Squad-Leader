// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCollectionDataInt.h"

#include "Components/EditableTextBox.h"

#include "Types/SlateEnums.h"

#include "TextCollectionDataItem.generated.h"

UCLASS()
class SQUADLEADER_API UTextCollectionDataItem : public UMenuCollectionDataInt
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* TextBoxData;

protected:
	virtual void SynchronizeProperties() override;

public:
	virtual void SetDataValue(int newValue) override;
	virtual void SetMinValue(int newValue) override;
	virtual void SetMaxValue(int newValue) override;
};
