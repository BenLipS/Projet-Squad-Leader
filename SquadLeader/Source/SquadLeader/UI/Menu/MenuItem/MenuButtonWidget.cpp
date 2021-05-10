// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuButtonWidget.h"

void UMenuButtonWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (MainButton)
	{
		MainButton->OnClicked.AddDynamic(this, &UMenuButtonWidget::OnButtonPressed);
		TextButton->SetText(FText::FromString(ButtonText));
	}
}

void UMenuButtonWidget::OnButtonPressed()
{
	OnButtonPressedEvent.Broadcast();
}
