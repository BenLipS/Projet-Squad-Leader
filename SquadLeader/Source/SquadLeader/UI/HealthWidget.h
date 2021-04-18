// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Engine/Canvas.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interface/PlayerHealthInterface.h"
#include "Interface/PlayerHealthDelegateInterface.h"
#include "HealthWidget.generated.h"
/**
 * 
 */
UCLASS()
class SQUADLEADER_API UHealthWidget : public USL_UserWidget, public IPlayerHealthInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* ProgressBarHP;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextHP;

	float Health = 100;
	float MaxHealth = 100;

public:
	UHealthWidget(const FObjectInitializer& ObjectInitializer);

	void SetupDelegateToObject_Implementation(UObject* ObjectIn) override;

	//-----IPlayerHealthInterface-----
	void OnPlayerHealthChanged(float newValue) override;
	void OnPlayerMaxHealthChanged(float newValue) override;
};