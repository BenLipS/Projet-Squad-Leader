// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"
#include "MenuItemWidget.generated.h"

class UMenuLayoutWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEventNoParam);

UCLASS()
class SQUADLEADER_API UMenuItemWidget : public USL_UserWidget
{
	friend class UMenuLayoutWidget;
	GENERATED_BODY()
protected:
	UMenuLayoutWidget* MenuLayout;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSelectGlobal = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSelected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SelectChannel = "Default";

	UFUNCTION(BlueprintCallable)
	void SelectItem();

	UFUNCTION(BlueprintCallable)
	bool DeselectItem();

public:
	UFUNCTION()
	bool IsSelected();

	UFUNCTION()
	virtual void OnItemSelected();

	UFUNCTION(BlueprintNativeEvent)
	void BlueprintOnItemSelected();
	void BlueprintOnItemSelected_Implementation() {}


	UFUNCTION()
	virtual void OnItemDeselected();

public:
	UFUNCTION(BlueprintCallable)
	UMenuLayoutWidget* GetMenuLayout();

public:
	UPROPERTY(BlueprintAssignable, Category = "Appearance|Event")
	FEventNoParam OnItemSelectedEvent;
	DECLARE_EVENT(UMenuItemWidget, FNativeEventNoParam);
	FNativeEventNoParam OnNativeItemSelectedEvent;

protected:
	/*Called after a MenuLayout is binded*/
	UFUNCTION(BlueprintNativeEvent, Category = "MenuWidgetItem")
	void OnItemAddedToLayout();
	virtual void OnItemAddedToLayout_Implementation();
};
