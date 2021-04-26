// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Engine/Canvas.h"
#include "ProgressBarStepWidget.h"
#include "Components/TextBlock.h"
#include "Interface/PlayerShieldInterface.h"
#include "ShieldWidget.generated.h"
/**
 *
 */
UCLASS()
class SQUADLEADER_API UShieldWidget : public USL_UserWidget, public IPlayerShieldInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBarStepWidget* ProgressBarShield;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextShield;

	float Shield = 100;
	float MaxShield = 100;

	/*How many HP a segment of the progressBar can hold*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShieldSegment = 50.f;

public:
	UShieldWidget(const FObjectInitializer& ObjectInitializer);

	void SetupDelegateToObject_Implementation(UObject* ObjectIn) override;

	//-----IPlayerShieldInterface-----

	void OnPlayerShieldChanged(float newValue) override;

	void OnPlayerMaxShieldChanged(float newValue) override;
};