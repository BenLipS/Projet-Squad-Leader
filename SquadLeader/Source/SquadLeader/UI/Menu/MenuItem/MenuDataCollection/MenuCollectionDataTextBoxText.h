// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCollectionDataText.h"

#include "Components/EditableTextBox.h"

#include "MenuCollectionDataTextBoxText.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuCollectionDataTextBoxText : public UMenuCollectionDataText
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* TextBoxData;

protected:
	virtual void SynchronizeProperties() override;
	
public:
	virtual bool SetDataValue(const FString& newValue, bool fireEvent = true) override;
};
