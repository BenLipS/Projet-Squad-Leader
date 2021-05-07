// Fill out your copyright notice in the Description page of Project Settings.


#include "SL_UserWidget.h"

int32 USL_UserWidget::GetDesiredZOrderInViewport()
{
	return DesiredZOrderInViewport;
}

USL_UserWidget::USL_UserWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
}

void USL_UserWidget::AddToViewport()
{
	Super::AddToViewport(DesiredZOrderInViewport);
}

void USL_UserWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
}
