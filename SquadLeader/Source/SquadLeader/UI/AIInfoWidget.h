// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blueprint/IUserObjectListEntry.h"
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
	UTextBlock* TextAI;

	float Health = 100;
	float MaxHealth = 100;

public:
	UAIInfoWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void OnHealthChanged(float newValue);

	UFUNCTION(BlueprintCallable)
	void OnMaxHealthChanged(float newValue);

	UObject* GetListItemObject();
};
