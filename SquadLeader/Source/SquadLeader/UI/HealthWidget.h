// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Engine/Canvas.h"
#include "ProgressBarStepWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Interface/PlayerHealthInterface.h"
#include "Interface/PlayerShieldInterface.h"
#include "HealthWidget.generated.h"
/**
 * 
 */
UCLASS()
class SQUADLEADER_API UHealthWidget : public USL_UserWidget, public IPlayerHealthInterface, public IPlayerShieldInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBarStepWidget* ProgressBarHP;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* TextHP;

	float Health = 100;
	float MaxHealth = 100;

	/*How many HP a segment of the progressBar can hold*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float HPSegment = 50.f;

	/*How many Shield a segment of the progressBar can hold*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBarStepWidget* ProgressBarShield;

	/*UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextShield;*/

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ShieldBackground;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShieldSegment = 50.f;

	float Shield = 100;
	float MaxShield = 100;

public:
	UHealthWidget(const FObjectInitializer& ObjectInitializer);

	void SetupDelegateToObject_Implementation(UObject* ObjectIn) override;

	//-----IPlayerHealthInterface-----
	void OnPlayerHealthChanged(float newValue) override;
	void OnPlayerMaxHealthChanged(float newValue) override;

	//-----IPlayerShieldInterface-----

	void OnPlayerShieldChanged(float newValue) override;

	void OnPlayerMaxShieldChanged(float newValue) override;
};