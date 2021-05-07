// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuItemWidget.h"
#include "Components/ScrollBox.h"
#include "../Interface/StatInfoInterface.h"

#include "MenuListInfo.generated.h"

class UMenuListInfoItemWidget;

UCLASS()
class SQUADLEADER_API UMenuListInfo : public UMenuItemWidget
{
	friend class UMenuListInfoItemWidget;
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	bool bAreItemsClickable = false;

	UPROPERTY()
	UMenuListInfoItemWidget* ItemSelected = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMenuListInfoItemWidget> ItemClass;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* ListEntry;


	UFUNCTION(BlueprintCallable)
	void AddEntryToList(UMenuListInfoItemWidget* Entry);

	UFUNCTION(BlueprintCallable)
	UMenuListInfoItemWidget* GetSelectedItem();

	//-----UMenuItemWidget-----
protected:
//	virtual void OnItemAddedToLayout_Implementation() override;
};
