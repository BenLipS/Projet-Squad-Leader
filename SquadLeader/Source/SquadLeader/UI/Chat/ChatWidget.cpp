// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatWidget.h"

#include "SquadLeader/Soldiers/Players/SoldierPlayerController.h"


#include "Components/TextBlock.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"

bool UChatWidget::IsChatOpen()
{
	return bIsChatOpen;
}

void UChatWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (IChatDelegateInterface* ChatDelegateInterface = Cast<IChatDelegateInterface>(ObjectIn); ChatDelegateInterface)
	{
		ChatDelegateInterface->AddChatDelegate(this);
	}
}

void UChatWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UChatWidget::ValidEntry()
{
	if (IsValid(ChatEntry))
	{
		if (ASoldierPlayerController* PC = GetOwningPlayer<ASoldierPlayerController>(); PC)
		{
			PC->OnChatMessageSent(ChatEntry->GetText().ToString());
		}
	}
	ExitChat();
}

void UChatWidget::CancelEntry()
{
	ExitChat();
}

void UChatWidget::ExitChat()
{
	if (IsValid(ChatEntry))
	{
		ChatEntry->SetText(FText::FromString(""));
	}

	MessageContainer->SetScrollBarVisibility(ESlateVisibility::Hidden);

	bIsChatOpen = false;
	if (ASoldierPlayerController* PC = GetOwningPlayer<ASoldierPlayerController>(); PC)
	{
		PC->SetShowMouseCursor(false);
		PC->ClientIgnoreLookInput(false);

		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(false);
		PC->SetInputMode(InputMode);
	}
}

void UChatWidget::OnChatMessageReceived(const FString& message)
{
	auto newEntry = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

	newEntry->SetText(FText::FromString(message));

	MessageContainer->AddChild(newEntry);
	MessageContainer->ScrollToEnd();
}

void UChatWidget::OnChatInputPressed()
{
	if (!bIsChatOpen)
	{
		if (auto PC = GetOwningPlayer<ASoldierPlayerController>(); PC)
		{
			PC->SetShowMouseCursor(true);
			PC->ClientIgnoreLookInput(true);
			//PC->ClientIgnoreMoveInput(true);

			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			InputMode.SetWidgetToFocus(ChatEntry->TakeWidget());

			PC->SetInputMode(InputMode);


			FVector2D Center = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / 2.0f;
			PC->SetMouseLocation(Center.X, Center.Y);

			MessageContainer->SetScrollBarVisibility(ESlateVisibility::Visible);

			bIsChatOpen = true;
		}
	}
}