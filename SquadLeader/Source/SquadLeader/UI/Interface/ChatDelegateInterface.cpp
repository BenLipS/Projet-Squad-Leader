// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatDelegateInterface.h"

// Add default functionality here for any IChatDelegateInterface functions that are not pure virtual.

void IChatDelegateInterface::OnChatMessageReceived(const FString& message)
{
	for (auto ChatInterface : ChatDelegates)
	{
		ChatInterface->OnChatMessageReceived(message);
	}
}

void IChatDelegateInterface::OnChatInputPressed()
{
	for (auto ChatInterface : ChatDelegates)
	{
		ChatInterface->OnChatInputPressed();
	}
}

void IChatDelegateInterface::AddChatDelegate(IChatInterface* newDelegate)
{
	ChatDelegates.AddUnique(newDelegate);
}

void IChatDelegateInterface::RemoveChatDelegate(IChatInterface* removeDelegate)
{
	ChatDelegates.Remove(removeDelegate);
}
