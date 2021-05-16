// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"
#include "MenuItemWidget.generated.h"

class UMenuLayoutWidget;

UCLASS()
class SQUADLEADER_API UMenuItemWidget : public USL_UserWidget
{
	friend class UMenuLayoutWidget;
	GENERATED_BODY()
protected:
	UMenuLayoutWidget* MenuLayout;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSelectGlobal = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SelectChannel = "Default";

	UFUNCTION(BlueprintCallable)
	void SelectItem();

	UFUNCTION()
	virtual void OnItemSelected();

	UFUNCTION()
	virtual void OnItemDeselected();

public:
	UFUNCTION(BlueprintCallable)
	UMenuLayoutWidget* GetMenuLayout();

protected:
	/*Called after a MenuLayout is binded*/
	UFUNCTION(BlueprintNativeEvent, Category = "MenuWidgetItem")
	void OnItemAddedToLayout();
	virtual void OnItemAddedToLayout_Implementation();
};
