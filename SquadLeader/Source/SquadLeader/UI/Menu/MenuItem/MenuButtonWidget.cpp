// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuButtonWidget.h"

void UMenuButtonWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (IsValid(MainButton))
	{
		MainButton->OnClicked.AddDynamic(this, &UMenuButtonWidget::OnButtonPressed);
	}

	if (IsValid(TextButton))
	{
		TextButton->SetText(FText::FromString(ButtonText));
	}
}

void UMenuButtonWidget::SetButtonText(FString newText)
{
	if (IsValid(TextButton))
	{
		TextButton->SetText(FText::FromString(newText));
	}
}

void UMenuButtonWidget::OnButtonPressed()
{
	OnButtonPressedEvent.Broadcast();
}
