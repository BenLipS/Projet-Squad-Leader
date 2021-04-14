#include "NotificationWidgetElement.h"
#include "NotificationWidgetElement.h"
#include "NotificationWidget.h"

UNotificationWidget* UNotificationWidgetElement::GetNotificationWidget()
{
	return NotificationWidget;
}

void UNotificationWidgetElement::BindItemToNotification(UNotificationWidget* newNotificationWidget)
{
	NotificationWidget = newNotificationWidget;
	OnItemAddedToNotificationEvent(newNotificationWidget);
}

void UNotificationWidgetElement::OnItemAddedToNotificationEvent_Implementation(UNotificationWidget* newNotificationWidget)
{

}