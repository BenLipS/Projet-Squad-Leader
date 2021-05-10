// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../MenuItemWidget.h"
#include "MenuDataCollectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuDataCollectionWidget : public UMenuItemWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UNamedSlot* DataSlot;

	virtual void SynchronizeProperties() override;

//-----UMenuItemWidget-----
protected:
	virtual void OnItemAddedToLayout_Implementation();
//-----UMenuItemWidget-----

public:
	UFUNCTION(BlueprintCallable)
	int GetData(FString Key);
};
