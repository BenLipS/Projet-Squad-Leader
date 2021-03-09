// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Components/TileView.h"
#include "AIInfoWidget.h"
#include "AIInfoListWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UAIInfoListWidget : public USL_UserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTileView* AIInfoTileView;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | AIInfo")
	TSubclassOf<class UAIInfoWidget> AIInfoWidgetClass;

	TArray<UAIInfoWidget*> ListWidget;

public:
	UAIInfoListWidget(const FObjectInitializer& ObjectInitializer);
	void AddItem(ASoldierAI* SoldierBound);
};
