// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuLayoutWidget.h"

#include "Components/HorizontalBox.h"

#include "RespawnMenuLayout.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API URespawnMenuLayout : public UMenuLayoutWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UHorizontalBox* RespawnBox;

protected:
	virtual void SynchronizeAllMenuItem() override;

private:
	template <typename T>
	void SynchronizeAllMenuItem(UPanelWidget* PanelIn);
	
public:
	UMenuItemWidget* AddChildToRespawnBox(TSubclassOf<UMenuItemWidget> newChild);
};
