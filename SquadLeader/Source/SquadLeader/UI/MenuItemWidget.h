// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "MenuItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuItemWidget : public USL_UserWidget
{
	friend class UMenuLayoutWidget;
	GENERATED_BODY()
protected:
	class UMenuLayoutWidget* MenuLayout;

public:
	UFUNCTION(BlueprintCallable)
	class UMenuLayoutWidget* GetMenuLayout();
};
