// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuItemWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "MenuButtonWidget.generated.h"

/**
 * 
 */

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonPressedEvent);

UCLASS()
class SQUADLEADER_API UMenuButtonWidget : public UMenuItemWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* MainButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextButton;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true), EditAnywhere)
	FString ButtonText;

protected:
	virtual void SynchronizeProperties() override;

private:
	UFUNCTION()
	void OnButtonPressed();

public:
	UPROPERTY(BlueprintAssignable, Category = "Appearance|Event")
	FOnButtonPressedEvent OnButtonPressedEvent;
	DECLARE_EVENT(UMenuButtonWidget, FNativeOnButtonPressedEvent);
	FNativeOnButtonPressedEvent OnNativeButtonPressedEvent;

};