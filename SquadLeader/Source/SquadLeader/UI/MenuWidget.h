// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuWidget : public USL_UserWidget
{
	GENERATED_BODY()
	

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
};
