// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "WheelWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UWheelWidget : public USL_UserWidget
{
	GENERATED_BODY()

public:
	UWheelWidget(const FObjectInitializer& ObjectInitializer);
	

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget | Wheel")
	uint8 NbElement;

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
	TArray<class USL_UserWidget*> ListItems;

	UPROPERTY(EditDefaultsOnly, Category = "Widget | Wheel")
	TArray<TSubclassOf<class USL_UserWidget>> Items;

public:
	const TArray<class USL_UserWidget*>& GetItems();

protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;
	
public:
	void Tick();
};