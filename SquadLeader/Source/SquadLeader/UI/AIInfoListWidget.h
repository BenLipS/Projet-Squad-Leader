// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Components/TileView.h"
#include "../Soldiers/AIs/SoldierAI.h"
#include "AIInfoListWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UAIInfoListWidget : public USL_UserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Widget | AIInfo")
	TArray<class UAIInfoWidget*> AIInfoWidgetList;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | AIInfo")
	TSubclassOf<class UAIInfoWidget> AIInfoWidgetClass;

public:
	UAIInfoListWidget(const FObjectInitializer& ObjectInitializer);

	//-----Squad-----
	UFUNCTION()
	void OnSquadChanged(const TArray<FSoldierAIData>& newValue);

	UFUNCTION()
	void OnSquadHealthChanged(int index, float newHealth);

	UFUNCTION()
	void OnSquadMaxHealthChanged(int index, float newHealth);

	UFUNCTION()
	void OnSquadShieldChanged(int index, float newShield);

	UFUNCTION()
	void OnSquadMaxShieldChanged(int index, float newMaxShield);
};
