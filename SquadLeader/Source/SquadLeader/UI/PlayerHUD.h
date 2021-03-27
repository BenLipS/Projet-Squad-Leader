// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "../Soldiers/AIs/SoldierAI.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
/*
//-----VALUES-----

//-----WIDGETS-----
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget | PlayerHealth")
	TSubclassOf<class UHealthWidget> HealthWidgetClass;

	class UHealthWidget* HealthWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | PlayerShield")
	TSubclassOf<class UShieldWidget> ShieldWidgetClass;

	class UShieldWidget* ShieldWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widget | AIInfo")
	TSubclassOf<class UAIInfoListWidget> AIListInfoWidgetClass;

	class UAIInfoListWidget* AIListInfoWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widget | PlayerAmmo")
	TSubclassOf<class UAmmoWidget> AmmoWidgetClass;

	class UAmmoWidget* AmmoWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widget | PlayerAmmo")
	TSubclassOf<class UWheelWidget> WheelWidgetClass;

	class UWheelWidget* WheelWidget;
	
public:
	APlayerHUD();

protected:
	virtual void BeginPlay() override;

public:
	/*Initialize all data from Owner controller PlayerState,
	should be called when changing player controller to relink datas
	void SetPlayerStateLink();

	void SetAIStateLink();

	virtual void DrawHUD() override;


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

	//-----SquadInfo-----
	UFUNCTION()
	void OnSquadChanged(const TArray<FSoldierAIData>& newValue);

	UFUNCTION()
	void OnSquadHealthChanged(int index, float newHealth);

	UFUNCTION()
	void OnSquadMaxHealthChanged(int index, float newHealth);

	UFUNCTION()
	void OnSquadShieldChanged(int index, float newShield);

	UFUNCTION()
	void OnSquadMaxShieldChanged(int index, float newMaxShield);
	
	UFUNCTION()
	void OnAmmoChanged(int8 newAmmo);

	UFUNCTION()
	void OnMaxAmmoChanged(int8 newMaxAmmo);

	UFUNCTION()
	void OnOrderInputPressed();

	UFUNCTION()
	void OnOrderInputReleased();*/
};
