// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "SquadLeader/SquadLeader.h"
#include "Animation/WidgetAnimation.h"

#include "CooldownElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UCooldownElementWidget : public USL_UserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ImageCooldown;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextKey;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextCooldown;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* CD_Ready;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* CD_Used;

	
	float TimeRemaining;
	float MaxTimeRemaining;

	bool bIsInCooldown;

	FTimerHandle CooldownTimerHandle;

	UPROPERTY()
	ESoldierAbilityInputID AbilityID = ESoldierAbilityInputID::None;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void OnCooldownEnded();

public:
	UFUNCTION()
	void LaunchPartialCooldown(float Timer, float MaxTimer);

	UFUNCTION()
	void LaunchCooldown(float Timer);

	UFUNCTION()
	void InitState(ESoldierAbilityInputID newKey, FString KeyText);

	UFUNCTION()
	ESoldierAbilityInputID GetAbilityID();
};
