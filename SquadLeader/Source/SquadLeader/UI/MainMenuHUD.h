// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SL_HUD.h"
#include "Interface/StatInfoDelegateInterface.h"
#include "Interface/MenuGameInfoDelegateInterface.h"

#include "MainMenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AMainMenuHUD : public ASL_HUD,
	public IStatInfoDelegateInterface,
	public IMenuGameInfoDelegateInterface
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	void GetProfileData();

//-----IStatInfoDelegateInterface-----
public:
	//void OnStatInfoReceived(FString Key, FString Value) override;
	//void OnStatsInfoReceived(TMap<FString, FString> statsIn) override;
};
