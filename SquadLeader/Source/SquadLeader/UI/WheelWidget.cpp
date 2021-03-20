// Fill out your copyright notice in the Description page of Project Settings.


#include "WheelWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UWheelWidget::UWheelWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

const TArray<class USL_UserWidget*>& UWheelWidget::GetItems()
{
	return ListItems;
}

int32 UWheelWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FVector2D CenterScreen = (UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / 2.0f) * (1.0f / UWidgetLayoutLibrary::GetViewportScale(GetWorld()));
	float AngleSection = 360.f / float(NbElement);

	//-----MOUSE_SELECTION-----
	int MouseIndex = -10;
	FVector2D RelativeMousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()) - CenterScreen;

	//Debug purpose
	UWidgetBlueprintLibrary::DrawLine(Context,
		CenterScreen,
		CenterScreen + RelativeMousePosition,
		FLinearColor::Red,
		true,
		2.f);

	if (RelativeMousePosition.Size() >= InnerCircleRadius)
	{
		MouseIndex = UKismetMathLibrary::GenericPercent_FloatFloat(UKismetMathLibrary::DegAtan2(RelativeMousePosition.Y, RelativeMousePosition.X) + 360.f, 360.f) / AngleSection;
	}

	//-----DRAW-----

	for (int i = 0; i < NbElement; i++)
	{
		if (i == MouseIndex || i == (MouseIndex + 1) % NbElement)
		{
			UWidgetBlueprintLibrary::DrawLine(Context,
				CenterScreen + (FVector2D(UKismetMathLibrary::DegCos(AngleSection * i), UKismetMathLibrary::DegSin(AngleSection * i)) * InnerCircleRadius),
				CenterScreen + (FVector2D(UKismetMathLibrary::DegCos(AngleSection * i), UKismetMathLibrary::DegSin(AngleSection * i)) * OutterCircleRadius),
				SeparationColor,
				true,
				SelectionThickness);
		}
		else
		{
			UWidgetBlueprintLibrary::DrawLine(Context,
				CenterScreen + (FVector2D(UKismetMathLibrary::DegCos(AngleSection * i), UKismetMathLibrary::DegSin(AngleSection * i)) * InnerCircleRadius),
				CenterScreen + (FVector2D(UKismetMathLibrary::DegCos(AngleSection * i), UKismetMathLibrary::DegSin(AngleSection * i)) * OutterCircleRadius),
				SeparationColor,
				true,
				DefaultThickness);
		}
		
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}