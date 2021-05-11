// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ChatInterface.h"
#include "ChatDelegateInterface.generated.h"

UINTERFACE(MinimalAPI)
class UChatDelegateInterface : public UChatInterface
{
	GENERATED_BODY()
};

class SQUADLEADER_API IChatDelegateInterface : public IChatInterface
{
	GENERATED_BODY()

private:
	TArray<IChatInterface*> ChatDelegates;

public:
	virtual void OnChatMessageReceived(const FString& message) override;

	virtual void OnChatInputPressed() override;

	void AddChatDelegate(IChatInterface* newDelegate);
	void RemoveChatDelegate(IChatInterface* removeDelegate);
};
