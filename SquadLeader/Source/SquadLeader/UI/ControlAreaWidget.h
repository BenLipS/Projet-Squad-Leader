// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Interface/ControlAreaInterface.h"

#include "Components/HorizontalBox.h"

#include "ControlAreaWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UControlAreaWidget : public USL_UserWidget, public IControlAreaInterface
{
	GENERATED_BODY()

protected:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn) override;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* AreaContainer;

protected:
	TArray<class UControlAreaInfoWidget*> ControlAreaList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UControlAreaInfoWidget> ControlAreaInfoClass;

protected:
	virtual void SynchronizeProperties() override;

//-----IControlAreaInterface-----
public:
	virtual void OnControlAreaInit(unsigned int nbArea) override;
	// Owner : -1 ennemy, 0 neutral, 1 ally
	virtual void OnAreaCaptureChanged(unsigned int index, int owner, int capturer, float capturePercent) override;
};
