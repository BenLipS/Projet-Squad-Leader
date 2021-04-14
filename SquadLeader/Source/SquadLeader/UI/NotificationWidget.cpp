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

void UNotificationWidget::AddNotification(UNotificationWidgetElement* newWidget)
{
	if (!newWidget)
		return;

	//if (IsInViewport())
	newWidget->AddToViewport();

	FNotificationWidgetSlot newItemSlot;
	newItemSlot.Widget = newWidget;

	newItemSlot.Slot = MainPanel->AddChildToCanvas(newItemSlot.Widget);

	newItemSlot.Slot->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
	newItemSlot.Slot->SetPosition(FVector2D(0.f, 0.f));
	newItemSlot.Slot->SetAlignment(FVector2D(0.f, 0.f));
	newItemSlot.Slot->SetAutoSize(true);

	Items.EmplaceAt(0, newItemSlot);

	UpdateNotificationsPosition();
}

void UNotificationWidget::UpdateNotificationsPosition()
{

	while (MaxItem > 0 && Items.Num() > MaxItem)
	{
		auto remove = Items.Pop();
		WidgetTree->RemoveWidget(remove.Widget);
	}

	int nbElement = 0;
	float YPosition = 0.f;

	for (auto pair : Items)
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
	UNotificationWidgetElementText* newTextElement = CreateWidget<UNotificationWidgetElementText>(GetWorld(), TextElementClass);
	newTextElement->InitText(textNotification);
	AddNotification(newTextElement);
}