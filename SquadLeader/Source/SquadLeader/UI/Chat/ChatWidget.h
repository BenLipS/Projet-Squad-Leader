// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../SL_UserWidget.h"

#include "../Interface/ChatDelegateInterface.h"

#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h"
#include "Engine/EngineTypes.h"

#include "ChatWidget.generated.h"

class UChatEntryWidget;

UCLASS()
class SQUADLEADER_API UChatWidget : public USL_UserWidget, public IChatInterface
{
	GENERATED_BODY()

protected:
	FTimerHandle ScrollTimer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* ChatEntry;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* MessageContainer;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* Background;

	UPROPERTY(BlueprintReadOnly)
	bool bIsChatOpen;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UChatEntryWidget> ChatEntryClass;

public:
	UFUNCTION(BlueprintCallable)
	bool IsChatOpen();

//-----USL_UserWidget-----
public:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn) override;

protected:
	virtual void SynchronizeProperties() override;

protected:
	UFUNCTION(BlueprintCallable)
	void ValidEntry();

	UFUNCTION(BlueprintCallable)
	void CancelEntry();

	void ExitChat();

//-----IChatInterface-----
public:
	virtual void OnChatMessageReceived(const FString& message) override;
	virtual void OnChatInputPressed() override;
};
