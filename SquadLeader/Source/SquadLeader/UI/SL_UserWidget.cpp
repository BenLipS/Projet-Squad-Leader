// Fill out your copyright notice in the Description page of Project Settings.


#include "SL_UserWidget.h"

USL_UserWidget::USL_UserWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
}

void USL_UserWidget::AddToViewport(int32 ZOrder)
{
	Super::AddToViewport(ZOrder);
}

void USL_UserWidget::SetupDelegateToObject(UObject* ObjectIn)
{
}
