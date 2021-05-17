// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuLayoutWidget.h"

#include "MenuWidget.h"
#include "MenuItemWidget.h"

#include "Components/PanelWidget.h"
#include "Components/Widget.h"

UMenuWidget* UMenuLayoutWidget::GetMenu()
{
	return Menu;
}

void UMenuLayoutWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UMenuLayoutWidget::SynchronizeAllMenuItem()
{
	if (MenuSlot)
	{
		for (auto childSlot : MenuSlot->GetAllChildren())
		{
			if (auto panelChild = Cast<UPanelWidget>(childSlot))
			{
				SynchronizeAllMenuItem<UMenuItemWidget>(panelChild);
			}
		}
	}
}

void UMenuLayoutWidget::SetWidgetLayout(UMenuItemWidget* ItemIn)
{
	if (IsValid(ItemIn))
	{
		MenuItems.AddUnique(ItemIn);
		ItemIn->MenuLayout = this;
		ItemIn->OnItemAddedToLayout();
	}
}

template<typename T>
void UMenuLayoutWidget::SynchronizeAllMenuItem(UPanelWidget* PanelIn)
{
	auto ListChildren = PanelIn->GetAllChildren();
	for (UWidget* Widget : ListChildren)
	{
		if (T* MenuItem = Cast<T>(Widget); MenuItem)
		{
			SetWidgetLayout(MenuItem);
		}
		else if (UPanelWidget* MenuGroup = Cast<UPanelWidget>(Widget); MenuGroup)
		{
			SynchronizeAllMenuItem<T>(MenuGroup);
		}
	}
}


/*void UMenuLayoutWidget::SynchronizeAllMenuItem(TArray<UWidget*> ListChildren)
{
	for (UWidget* Widget : ListChildren)
	{
		if (UMenuItemWidget* MenuItem = Cast<UMenuItemWidget>(Widget); MenuItem)
		{
			MenuItems.AddUnique(MenuItem);
			MenuItem->MenuLayout = this;
			MenuItem->OnItemAddedToLayout();
		}
		else if (UPanelWidget* MenuGroup = Cast<UPanelWidget>(Widget); MenuGroup)
		{
			SynchronizeAllMenuItem(MenuGroup->GetAllChildren());
		}
	}
}*/

void UMenuLayoutWidget::OnItemSelected(UMenuItemWidget* newItem)
{
	if (IsValid(newItem))
	{
		if (MenuItems.Contains(newItem))
		{
			if (SelectedItem.Contains(newItem->SelectChannel))
			{
				if (IsValid(SelectedItem[newItem->SelectChannel]))
				{
					SelectedItem[newItem->SelectChannel]->OnItemDeselected();
				}
				SelectedItem[newItem->SelectChannel] = newItem;
			}
			else
			{
				SelectedItem.Add(newItem->SelectChannel, newItem);
			}

			SelectedItem[newItem->SelectChannel]->OnItemSelected();
		}
		else
		{
			//Should never be here but in case
			return;
		}
	}
}

void UMenuLayoutWidget::DeselectChannel(FString ChannelIn)
{
	if (SelectedItem.Contains(ChannelIn))
	{
		if (IsValid(SelectedItem[ChannelIn]))
		{
			SelectedItem[ChannelIn]->OnItemDeselected();
			SelectedItem[ChannelIn] = nullptr;
		}
	}
}

void UMenuLayoutWidget::DeselectItem(UMenuItemWidget* newItem)
{
	if (IsValid(newItem) && SelectedItem.Contains(newItem->SelectChannel) && SelectedItem[newItem->SelectChannel] == newItem)
	{
		newItem->OnItemDeselected();
		SelectedItem[newItem->SelectChannel] = nullptr;
	}
}

UMenuItemWidget* UMenuLayoutWidget::GetSelectedItem(FString ChannelIn)
{
	if (SelectedItem.Contains(ChannelIn))
	{
		return SelectedItem[ChannelIn];
	}
	else
	{
		return nullptr;
	}
}

FString UMenuLayoutWidget::GetLayoutID()
{
	return GetName();
}
