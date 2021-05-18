// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationWidget.h"

#include "../Interface/NotificationDelegateInterface.h"

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

	newItemSlot.Slot = MainPanel->AddChildToVerticalBox(newItemSlot.Widget);

	Items.EmplaceAt(0, newItemSlot);

	UpdateNotificationsPosition();
}

void UNotificationWidget::UpdateNotificationsPosition()
{
	while (MaxItem > 0 && Items.Num() > MaxItem)
	{
		auto remove = Items.Pop();
		MainPanel->RemoveChild(remove.Widget);
		WidgetTree->RemoveWidget(remove.Widget);
	}

	/*float YPosition = 0.f;

	for (auto pair : Items)
	{
		pair.Slot->SetPosition(FVector2D(400.f, YPosition));
		YPosition += pair.Slot->GetSize().Y;
	}*/
}

void UNotificationWidget::SetupDelegateToObject_Implementation(UObject* ObjectIn)
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