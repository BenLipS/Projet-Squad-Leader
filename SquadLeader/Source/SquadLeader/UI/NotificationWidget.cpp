// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationWidget.h"

#include "Interface/NotificationDelegateInterface.h"

#include "NotificationWidgetElement.h"
#include "NotificationWidgetElementText.h"

#include "Blueprint/WidgetTree.h"

#include "Blueprint/UserWidget.h"

UNotificationWidget::UNotificationWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UNotificationWidget::AddNotification(FString textNotification)
{
	FTextBlockSlot newSlot;
	newSlot.TextBlock = WidgetTree->ConstructWidget<UTextBlock>();
	newSlot.TextBlock->SetJustification(ETextJustify::Right);
	newSlot.TextBlock->SetText(FText::FromString(textNotification));

	newSlot.Slot = MainPanel->AddChildToCanvas(newSlot.TextBlock);
	newSlot.Slot->SetAnchors(FAnchors(1.f, 0.7f, 1.f, 0.7f));
	newSlot.Slot->SetPosition(FVector2D(-5.f, 0.f));
	newSlot.Slot->SetAlignment(FVector2D(1.f, 1.f));
	newSlot.Slot->SetAutoSize(true);
	Items.EmplaceAt(0, newSlot);

	UpdateNotificationsPosition();
}

void UNotificationWidget::UpdateNotificationsPosition()
{

	while (MaxItem > 0 && Items.Num() > MaxItem)
	{
		auto remove = Items.Pop();
		WidgetTree->RemoveWidget(remove.TextBlock);
	}

	int nbElement = 0;
	float YPosition = 0.f;

	for (FTextBlockSlot pair : Items)
	{
		pair.Slot->SetPosition(FVector2D(0.f, YPosition));
		YPosition -= pair.Slot->GetSize().Y;
	}
}

void UNotificationWidget::SetupDelegateToObject(UObject* ObjectIn)
{
	if (INotificationDelegateInterface* NotificationDelegateInterface = Cast<INotificationDelegateInterface>(ObjectIn); NotificationDelegateInterface)
	{
		NotificationDelegateInterface->AddNotificationDelegate(this);
	}
}

void UNotificationWidget::OnTextNotification_Received(FString textNotification)
{
	AddNotification(textNotification);
}