// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatWidget.h"

#include "SquadLeader/Soldiers/Players/SoldierPlayerController.h"

#include "ChatEntryWidget.h"

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
	Background->SetVisibility(ESlateVisibility::Hidden);

	if (IsValid(MessageContainer))
	{
		for (auto ChatElement : MessageContainer->GetAllChildren())
		{
			if (auto ChatItem = Cast<UChatEntryWidget>(ChatElement))
			{
				ChatItem->TryPlayFadeAnimation();
			}
		}
	}
}

void UChatWidget::OnChatMessageReceived(const FString& message)
{
	auto newEntry = WidgetTree->ConstructWidget<UChatEntryWidget>(ChatEntryClass);

	newEntry->SetEntryText(message);
	auto slot = MessageContainer->AddChild(newEntry);
	newEntry->SetStayTime(5.f);
	newEntry->ChatOwner = this;
	MessageContainer->ScrollToEnd();
	GetWorld()->GetTimerManager().SetTimer(ScrollTimer, MessageContainer, &UScrollBox::ScrollToEnd, 0.01f);
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

			Background->SetVisibility(ESlateVisibility::HitTestInvisible);
			if (IsValid(MessageContainer))
			{
				for (auto ChatElement : MessageContainer->GetAllChildren())
				{
					if (auto ChatItem = Cast<UChatEntryWidget>(ChatElement))
					{
						ChatItem->SetVisibility(ESlateVisibility::HitTestInvisible);
						ChatItem->CancelFadeAnimation();
					}
				}
			}
		}
	}
}