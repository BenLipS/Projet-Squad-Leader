// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../SL_UserWidget.h"
#include "../Interface/OrderInterface.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "WheelWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UWheelWidget : public USL_UserWidget, public IOrderInterface
{
	GENERATED_BODY()

public:
	UWheelWidget(const FObjectInitializer& ObjectInitializer);
	void SetupDelegateToObject(UObject* ObjectIn) override;

protected:
	virtual void SynchronizeProperties() override;

protected:
	//-----UI-----
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* Background;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USizeBox* ImageBox;

	//-----parameter-----
	UPROPERTY(EditDefaultsOnly, Category = "Widget | Wheel")
	FLinearColor BackgroundColor = FLinearColor::Black;

	UPROPERTY(BlueprintReadOnly, Category = "Widget | Wheel")
	uint8 NbElement;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | Wheel")
	float ItemHoverScale = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | Wheel")
	float InnerCircleRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | Wheel")
	float OutterCircleRadius = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | Wheel")
	float DefaultThickness = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | Wheel")
	float SelectionThickness = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | Wheel")
	FLinearColor SeparationColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category = "Widget | Wheel")
	TArray<class UWheelWidgetElement*> ListItems;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | Wheel")
	TArray<TSubclassOf<class UWheelWidgetElement>> ItemsDefault;

public:
	const TArray<class UWheelWidgetElement*>& GetItems();

	void AddToViewport(int32 ZOrder = 0) override;
	void RemoveFromViewport();

protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

	//-----IOrderInterface-----
public:
	void OnOrderInputPressed() override;
	void OnOrderInputReleased() override;
};