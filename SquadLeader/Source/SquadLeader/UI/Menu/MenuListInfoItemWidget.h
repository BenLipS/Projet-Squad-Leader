// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"
#include "Components/TextBlock.h"
#include "Containers/Map.h"

#include "MenuListInfoItemWidget.generated.h"

/**
 * 
 */
class UMenuListInfo;
UCLASS()
class SQUADLEADER_API UMenuListInfoItemWidget : public USL_UserWidget
{
	friend class UMenuListInfo;
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* LeftData;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RightData;

	UMenuListInfo* OwnerList;
	
public:
	void SetDatas(FString left, FString right);
	void SetLeftData(FString left);
	void SetRightData(FString right);

	TPair<FString, FString> GetDatas();

	UFUNCTION(BlueprintCallable)
	FString GetLeftData();

	UFUNCTION(BlueprintCallable)
	FString GetRightData();

protected:
	UFUNCTION(BlueprintCallable)
	void OnItemSelect();

	UFUNCTION(BlueprintCallable)
	void OnItemDeselect();
};
