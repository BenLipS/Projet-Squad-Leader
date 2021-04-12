// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../SL_UserWidget.h"
#include "../Interface/StatInfoDelegateInterface.h"
#include "../Interface/StatInfoInterface.h"

#include "MenuWidget.generated.h"

UCLASS()
class SQUADLEADER_API UMenuWidget : public USL_UserWidget, public IStatInfoDelegateInterface
{
	GENERATED_BODY()

public:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* MainPanel;

	UPROPERTY(BlueprintReadOnly)
	TArray<class UMenuLayoutWidget*> MenuLayouts;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true), EditAnywhere, Category = "Menu")
	FString DefaultLayout = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu | Debug")
	bool ShowAllLayout = false;

	UPROPERTY(BlueprintReadOnly)
	class UMenuLayoutWidget* CurrentLayout;

protected:
	virtual void SynchronizeProperties() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentLayout(FString layoutID);

//-----IStatInfoInterface-----
public:
//	virtual void OnStatInfoReceived(FString Key, FString Value) override;
//	virtual void OnStatsInfoReceived(TMap<FString, FString> statsIn) override;
};
