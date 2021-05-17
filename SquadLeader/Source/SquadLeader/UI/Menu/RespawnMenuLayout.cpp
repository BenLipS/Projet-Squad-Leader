// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnMenuLayout.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"

#include "MenuItemWidget.h"

void URespawnMenuLayout::SynchronizeAllMenuItem()
{
	Super::SynchronizeAllMenuItem();
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

template<typename T>
void URespawnMenuLayout::SynchronizeAllMenuItem(UPanelWidget* PanelIn)
{
	if (auto PanelHorizontalBox = Cast<UHorizontalBox>(PanelIn); PanelIn->GetName() == "RespawnBox" && IsValid(PanelHorizontalBox))
	{
		RespawnBox = PanelHorizontalBox;
		return;
	}

	auto ListChildren = PanelIn->GetAllChildren();
	for (UWidget* Widget : ListChildren)
	{
		if (UPanelWidget* MenuGroup = Cast<UPanelWidget>(Widget); MenuGroup)
		{
			SynchronizeAllMenuItem<T>(MenuGroup);
		}
	}
}

UMenuItemWidget* URespawnMenuLayout::AddChildToRespawnBox(TSubclassOf<UMenuItemWidget> newChild)
{
	if (MenuSlot->GetAllChildren().IsValidIndex(0))
	{
		if (IsValid(RespawnBox))
		{
			UMenuItemWidget* newWidget = WidgetTree->ConstructWidget<UMenuItemWidget>(newChild);
			
			RespawnBox->AddChildToHorizontalBox(newWidget);

			SetWidgetLayout(newWidget);

			return newWidget;
		}
	}
	return nullptr;
}