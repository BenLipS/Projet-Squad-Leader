// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuItemWidget.h"
#include "Components/ScrollBox.h"
#include "../Interface/StatInfoInterface.h"

#include "MenuListInfo.generated.h"

UCLASS()
class SQUADLEADER_API UMenuListInfo : public UMenuItemWidget, public IStatInfoInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMenuListInfoItemWidget> ItemClass;

	/*UPROPERTY(BlueprintReadOnly)
	TArray<class UMenuListInfoItemWidget*> Items;*/

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* ListEntry;

	void AddEntryToList(class UMenuListInfoItemWidget* Entry);

	//-----UMenuItemWidget-----
protected:
	virtual void OnItemAddedToLayout_Implementation() override;

	//-----IStatInfoInterface-----
public:
	virtual void OnStatInfoReceived(FString Key, FString Value) override;
	virtual void OnStatsInfoReceived(TMap<FString, FString> statsIn) override;
	virtual void OnStatsInfoCleanOrder() override;
};
