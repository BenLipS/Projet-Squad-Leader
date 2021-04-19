// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Components/VerticalBox.h"
#include "../Soldiers/AIs/SoldierAI.h"
#include "Interface/SquadInterface.h"
#include "AIInfoListWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UAIInfoListWidget : public USL_UserWidget, public ISquadInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Widget | AIInfo")
	TArray<class UAIInfoWidget*> AIInfoWidgetList;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | AIInfo")
	TSubclassOf<class UAIInfoWidget> AIInfoWidgetClass;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* AIInfoContainer;

public:
	UAIInfoListWidget(const FObjectInitializer& ObjectInitializer);

	void SetupDelegateToObject_Implementation(UObject* ObjectIn) override;

	//-----ISquadDelegateInterface-----
public:
	void OnSquadChanged(const TArray<FSoldierAIData>& newValue) override;

	void OnSquadHealthChanged(int index, float newHealth) override;

	void OnSquadMaxHealthChanged(int index, float newHealth) override;

	void OnSquadShieldChanged(int index, float newShield) override;

	void OnSquadMaxShieldChanged(int index, float newMaxShield) override;
};
