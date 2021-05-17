// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuButtonWidget.h"
#include "MenuButtonControlAreaWidget.generated.h"

class AControlArea;
class UControlAreaInfoWidget;
class UImage;

UCLASS()
class SQUADLEADER_API UMenuButtonControlAreaWidget : public UMenuButtonWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UControlAreaInfoWidget* ControlAreaWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* Background;

	UPROPERTY(BlueprintReadOnly)
	AControlArea* ControlArea;

private:
	int ControlAreaOwner = 0;

protected:
	UFUNCTION()
	void SetControlAreaOwner(int newOwner);

	virtual void OnItemSelected() override;
	virtual void OnItemDeselected() override;

public:
	int GetControlAreaOwner();


public:
	void SetControlArealink(AControlArea* ControlAreaIn);

	UFUNCTION()
	AControlArea* GetControlArea();

protected:
	virtual void SynchronizeProperties() override;
};
