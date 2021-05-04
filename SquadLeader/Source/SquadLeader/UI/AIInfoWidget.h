// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SquadLeader/AI/Mission.h"
#include "SquadLeader/Soldiers/Soldier.h"
#include "Materials/MaterialInterface.h"
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
	UImage* ClassImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* StatusImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ImageMission;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIInfoWidget | Mission")
	USlateBrushAsset* AttackingImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIInfoWidget | Mission")
	USlateBrushAsset* FormationImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIInfoWidget | Mission")
	USlateBrushAsset* DefaultImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIInfoWidget | Status")
	UMaterialInstance* StatusMaterialClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIInfoWidget | Assault")
	FLinearColor AssaultColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIInfoWidget | Assault")
	USlateBrushAsset* AssaultIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIInfoWidget | Heavy")
	FLinearColor HeavyColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIInfoWidget | Heavy")
	USlateBrushAsset* HeavyIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIInfoWidget | Support")
	FLinearColor SupportColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIInfoWidget | Support")
	USlateBrushAsset* SupportIcon;

	UPROPERTY()
	UMaterialInstanceDynamic* StatusMaterial;

	float Health = 100;
	float MaxHealth = 100;

	float Shield = 100;
	float MaxShield = 100;

	AIBasicState Mission = AIBasicState::Search;

	SoldierClass AIClass = SoldierClass::NONE;

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

	UFUNCTION(BlueprintCallable)
	void OnClassChanged(SoldierClass newValue);

protected:
	UFUNCTION()
	USlateBrushAsset* GetBrushFromClass(SoldierClass classIn);

	UFUNCTION()
	FLinearColor GetColorFromClass(SoldierClass classIn);

	UFUNCTION()
	USlateBrushAsset* GetBrushFromMission(AIBasicState missionIn);

	virtual void SynchronizeProperties() override;
};
