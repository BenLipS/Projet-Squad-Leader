// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"
#include "Components/TextBlock.h"

#include "MenuListInfoItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuListInfoItemWidget : public USL_UserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* LeftData;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RightData;
	
public:
	void SetDatas(FString left, FString right);
	void SetLeftData(FString left);
	void SetRightData(FString right);

	TPair<FString, FString> GetDatas();
	FString GetLeftData();
	FString GetRightData();
};
