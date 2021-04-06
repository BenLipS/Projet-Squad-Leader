// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotificationWidgetElement.h"

#include "Components/TextBlock.h"

#include "NotificationWidgetElementText.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UNotificationWidgetElementText : public UNotificationWidgetElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextNotification;

public:
	UFUNCTION(BlueprintCallable)
	void InitText(FString newText);
};
/*	FTextBlockSlot newSlot;
	newSlot.TextBlock = WidgetTree->ConstructWidget<UTextBlock>();
	newSlot.TextBlock->SetJustification(ETextJustify::Right);
	newSlot.TextBlock->SetText(FText::FromString(textNotification));

	newSlot.Slot = MainPanel->AddChildToCanvas(newSlot.TextBlock);
	newSlot.Slot->SetAnchors(FAnchors(1.f, 0.7f, 1.f, 0.7f));
	newSlot.Slot->SetPosition(FVector2D(-5.f, 0.f));
	newSlot.Slot->SetAlignment(FVector2D(1.f, 1.f));
	newSlot.Slot->SetAutoSize(true);
	Items.EmplaceAt(0, newSlot);*/