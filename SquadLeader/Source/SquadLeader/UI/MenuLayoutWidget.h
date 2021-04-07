// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Components/NamedSlot.h"

#include "MenuLayoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuLayoutWidget : public USL_UserWidget
{
	friend class UMenuWidget;
	GENERATED_BODY()

protected:
	class UMenuWidget* Menu;

public:
	UFUNCTION(BlueprintCallable)
	class UMenuWidget* GetMenu();

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UNamedSlot* MenuSlot;

	UPROPERTY(BlueprintReadOnly)
	TArray<class UMenuItemWidget*> MenuItems;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString LayoutID;

protected:
	virtual void SynchronizeProperties() override;

private:
	void SynchronizeAllMenuItem(TArray<class UWidget*> WidgetList);

public:
	FString GetLayoutID();
};
