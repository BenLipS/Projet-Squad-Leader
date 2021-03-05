// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget | Health")
	TSubclassOf<class UHealthWidget> HealthWidgetClass;

	class UHealthWidget* HealthWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | Health")
	TSubclassOf<class UShieldWidget> ShieldWidgetClass;

	class UShieldWidget* ShieldWidget;
	
public:
	APlayerHUD();

public:

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	//-----HP-----
	UFUNCTION(BlueprintCallable)
	void OnHealthChanged(float newValue);

	UFUNCTION(BlueprintCallable)
	void OnMaxHealthChanged(float newValue);

	//-----Shield-----
	UFUNCTION(BlueprintCallable)
	void OnShieldChanged(float newValue);

	UFUNCTION(BlueprintCallable)
	void OnMaxShieldChanged(float newValue);
};
