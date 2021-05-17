// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"
#include "Components/NamedSlot.h"

#include "MenuLayoutWidget.generated.h"

class UMenuWidget;
class UMenuItemWidget;
class UPanelWidget;

UCLASS()
class SQUADLEADER_API UMenuLayoutWidget : public USL_UserWidget
{
	friend class UMenuWidget;
	friend class UMenuItemWidget;
	GENERATED_BODY()

protected:
	UMenuWidget* Menu;

public:
	UFUNCTION(BlueprintCallable)
	UMenuWidget* GetMenu();

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UNamedSlot* MenuSlot;

	UPROPERTY(BlueprintReadOnly)
	TArray<UMenuItemWidget*> MenuItems;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, UMenuItemWidget*> SelectedItem;

protected:
	virtual void SynchronizeProperties() override;

protected:
	virtual void SynchronizeAllMenuItem();

	void SetWidgetLayout(UMenuItemWidget* ItemIn);

private:
	template <typename T>
	void SynchronizeAllMenuItem(UPanelWidget* PanelIn);

public:
	virtual void OnItemSelected(UMenuItemWidget* newItem);

	virtual void DeselectChannel(FString ChannelIn);

	virtual void DeselectItem(UMenuItemWidget* newItem);

	UFUNCTION(BlueprintCallable)
	UMenuItemWidget* GetSelectedItem(FString ChannelIn);

	FString GetLayoutID();
};