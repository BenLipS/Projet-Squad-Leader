// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMainMenuWidget : public UMenuWidget, public IStatInfoDelegateInterface
{
	GENERATED_BODY()

public:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn);

//-----IStatInfoInterface-----
public:
	//	virtual void OnStatInfoReceived(FString Key, FString Value) override;
	//	virtual void OnStatsInfoReceived(TMap<FString, FString> statsIn) override;
};
