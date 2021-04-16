// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaWidget.h"

#include "Interface/ControlAreaDelegateInterface.h"

#include "ControlAreaInfoWidget.h"
#include "Blueprint/WidgetTree.h"

void UControlAreaWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (IControlAreaDelegateInterface* ControlAreaDelegateInterface = Cast<IControlAreaDelegateInterface>(ObjectIn); ControlAreaDelegateInterface)
	{
		ControlAreaDelegateInterface->AddControlAreaDelegate(this);
	}
}

void UControlAreaWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (IsDesignTime())
	{
		if (ControlAreaInfoClass)
		{
			UControlAreaInfoWidget* newWidget = WidgetTree->ConstructWidget<UControlAreaInfoWidget>(ControlAreaInfoClass);
			newWidget->OnControlAreaChange(0, 1, 0.4f);
			ControlAreaList.Add(newWidget);
			AreaContainer->AddChildToHorizontalBox(newWidget);
		}
	}
}

void UControlAreaWidget::OnControlAreaInit(unsigned int nbArea)
{
	for (unsigned int i = 0; i < nbArea; i++)
	{
		if (ControlAreaInfoClass)
		{
			UControlAreaInfoWidget* newWidget = WidgetTree->ConstructWidget<UControlAreaInfoWidget>(ControlAreaInfoClass);
			newWidget->OnControlAreaChange(0, 0, 0.f);
			ControlAreaList.Add(newWidget);
			AreaContainer->AddChildToHorizontalBox(newWidget);
		}
	}
}

void UControlAreaWidget::OnAreaCaptureChanged(unsigned int index, int owner, int capturer, float capturePercent)
{
	if (ControlAreaList.IsValidIndex(index))
	{
		ControlAreaList[index]->OnControlAreaChange(owner, capturer, capturePercent);
	}
}
