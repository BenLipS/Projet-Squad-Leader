// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuListInfoItemWidget.h"
#include "MenuListGameItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuListGameItemWidget : public UMenuListInfoItemWidget
{
	GENERATED_BODY()
protected:
	FString IPAdress = "";

public:
	void SetIPAdress(FString AdressIn);

	UFUNCTION(BlueprintCallable)
	FString GetIPAdress();

protected:
	virtual void OnItemSelect() override;
	virtual void OnItemDeselect() override;
};
