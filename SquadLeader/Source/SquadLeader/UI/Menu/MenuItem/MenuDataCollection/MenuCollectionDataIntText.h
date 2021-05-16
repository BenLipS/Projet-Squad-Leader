// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCollectionDataInt.h"

#include "Components/TextBlock.h"

#include "MenuCollectionDataIntText.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuCollectionDataIntText : public UMenuCollectionDataInt
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextInt;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DescriptionText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Description;

protected:
	virtual void SynchronizeProperties() override;

public:
	virtual bool SetDataValue(int newValue, bool fireEvent = true) override;
};
