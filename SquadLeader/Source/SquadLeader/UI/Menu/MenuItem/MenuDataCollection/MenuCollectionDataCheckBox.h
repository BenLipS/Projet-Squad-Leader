// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MenuCollectionDataBool.h"

#include "Components/CheckBox.h"

#include "MenuCollectionDataCheckBox.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuCollectionDataCheckBox : public UMenuCollectionDataBool
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCheckBox* CheckBoxData;

protected:
	virtual void SynchronizeProperties() override;

	UFUNCTION()
	void OnCheckBoxChanged(bool newValue);

public:
	virtual bool SetDataValue(bool newValue) override;
};
