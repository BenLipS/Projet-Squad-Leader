// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SquadLeader/AI/Mission.h"
#include "AIInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UAIInfoWidget : public USL_UserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* ProgressBarHP;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* ProgressBarShield;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextAI;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ImageMission;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USlateBrushAsset* AttackingImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USlateBrushAsset* FormationImage;

	float Health = 100;
	float MaxHealth = 100;

	float Shield = 100;
	float MaxShield = 100;

	AIBasicState Mission = AIBasicState::Formation;

public:
	UAIInfoWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void OnHealthChanged(float newValue);

	UFUNCTION(BlueprintCallable)
	void OnMaxHealthChanged(float newValue);
	
	UFUNCTION(BlueprintCallable)
	void OnShieldChanged(float newValue);

	UFUNCTION(BlueprintCallable)
	void OnMaxShieldChanged(float newValue);

	UFUNCTION(BlueprintCallable)
	void OnMissionChanged(AIBasicState newValue);

protected:
	USlateBrushAsset* GetBrushFromMission(AIBasicState missionIn);
};
