// Fill out your copyright notice in the Description page of Project Settings.


#include "ClassSelectionMenuLayout.h"
#include "../MenuItem/MenuList/MenuListClass.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Blueprint/WidgetTree.h"

UMenuListClass* UClassSelectionMenuLayout::AddClassEntryToClassBox()
{
	if (IsValid(ClassBox))
	{
		UHorizontalBox* newBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

		UMenuListClass* newWidget = WidgetTree->ConstructWidget<UMenuListClass>(ClassItem);

		UTextBlock* newTextBlock = WidgetTree->ConstructWidget<UTextBlock>(DescriptionClass);
		++NbAI;
		newTextBlock->SetText(FText::FromString("AI Squad " + FString::FromInt(NbAI)));

		auto slotText = newBox->AddChildToHorizontalBox(newTextBlock);
		slotText->Size.SizeRule = ESlateSizeRule::Fill;
		slotText->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

		auto slotWidget = newBox->AddChildToHorizontalBox(newWidget);
		slotWidget->Size.SizeRule = ESlateSizeRule::Fill;
		slotWidget->SetPadding(FMargin(0.f, 5.f));

		auto slotLine = ClassBox->AddChild(newBox);

		SetWidgetLayout(newWidget);

		ListAIClass.Add(newWidget);

		return newWidget;
	}
	return nullptr;
}

TArray<SoldierClass> UClassSelectionMenuLayout::GetAIClasslist()
{
	TArray<SoldierClass> ret;
	for (auto classWidget : ListAIClass)
	{
		ret.Add(classWidget->GetCurrentClass());
	}
	return ret;
}

SoldierClass UClassSelectionMenuLayout::GetPlayerClass()
{
	if (IsValid(PlayerClass))
	{
		return PlayerClass->GetCurrentClass();
	}
	return SoldierClass::NONE;
}

void UClassSelectionMenuLayout::SynchronizeProperties()
{
	UMenuLayoutWidget::SynchronizeProperties();
	if (IsDesignTime())
	{
		NbAI = 0;
		if (IsValid(ClassBox))
		{
			ClassBox->ClearChildren();
		}
		for (int i = 0; i < NbClassPreview; i++)
		{
			AddClassEntryToClassBox();
		}
	}
}

void UClassSelectionMenuLayout::SynchronizeAllMenuItem()
{
	UMenuLayoutWidget::SynchronizeAllMenuItem();
	if (MenuSlot)
	{
		for (auto childSlot : MenuSlot->GetAllChildren())
		{
			if (auto panelChild = Cast<UPanelWidget>(childSlot))
			{
				SynchronizeAllMenuItem(panelChild);
			}
		}
	}

	if (IsDesignTime())
	{
		NbAI = 0;
		if (IsValid(ClassBox))
		{
			ClassBox->ClearChildren();
		}
		for (int i = 0; i < NbClassPreview; i++)
		{
			AddClassEntryToClassBox();
		}
	}
}

void UClassSelectionMenuLayout::SynchronizeAllMenuItem(UPanelWidget* PanelIn)
{
	if (auto PanelScrollBox = Cast<UScrollBox>(PanelIn); PanelIn->GetName() == "ClassBox" && IsValid(PanelScrollBox))
	{
		ClassBox = PanelScrollBox;
	}

	auto ListChildren = PanelIn->GetAllChildren();
	for (UWidget* Widget : ListChildren)
	{
		if (UPanelWidget* MenuGroup = Cast<UPanelWidget>(Widget); MenuGroup)
		{
			SynchronizeAllMenuItem(MenuGroup);
		}
		else if (auto PlayerClassWidget = Cast<UMenuListClass>(Widget); PanelIn->GetName() == "PlayerClass" && IsValid(PlayerClassWidget))
		{
			PlayerClass = PlayerClassWidget;
		}
	}
}