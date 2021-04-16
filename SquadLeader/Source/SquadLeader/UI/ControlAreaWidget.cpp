// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaWidget.h"

#include "Interface/MinimapDelegateInterface.h"

#include "ControlAreaInfoWidget.h"
#include "Blueprint/WidgetTree.h"

void UControlAreaWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (IMinimapDelegateInterface* MinimapDelegateInterface = Cast<IMinimapDelegateInterface>(ObjectIn); MinimapDelegateInterface)
	{
		MinimapDelegateInterface->AddMinimapDelegate(this);
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

void UControlAreaWidget::OnControlAreaAdded(AControlArea* _ControlArea)
{
	if (ControlAreaInfoClass)
	{
		UControlAreaInfoWidget* newWidget = WidgetTree->ConstructWidget<UControlAreaInfoWidget>(ControlAreaInfoClass);
		newWidget->OnControlAreaChange(0, 0, 0.f);
		ControlAreaList.Add(newWidget);
		AreaContainer->AddChildToHorizontalBox(newWidget);

		_ControlArea->OnControlAreaChanged.AddDynamic(newWidget, &UControlAreaInfoWidget::OnControlAreaChange);
	}
}