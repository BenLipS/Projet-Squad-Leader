// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

#include "MenuLayoutWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"

#include "Components/CanvasPanel.h"

void UMenuWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MainPanel)
	{
		for (UWidget* Widget : MainPanel->GetAllChildren())
		{
			if (UMenuLayoutWidget* MenuLayout = Cast<UMenuLayoutWidget>(Widget); MenuLayout)
			{
				MenuLayouts.AddUnique(MenuLayout);
				MenuLayout->Menu = this;
				MenuLayout->SynchronizeAllMenuItem();
				if (MenuLayout->GetLayoutID() != DefaultLayout && !IsDesignTime())
				{
					MenuLayout->SetVisibility(ESlateVisibility::Collapsed);
				}
				else
				{
					CurrentLayout.Add(MenuLayout);
				}
			}
		}
	}
}

void UMenuWidget::SetCurrentLayout(FString newlayoutID)
{
	if (auto newLayouts = MenuLayouts.FilterByPredicate([&newlayoutID](UMenuLayoutWidget* layout) {return layout->GetLayoutID() == newlayoutID; }); newLayouts.Num() != 0)
	{
		if (!IsDesignTime())
		{
			for (auto oldLayouts : CurrentLayout)
			{
				oldLayouts->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		CurrentLayout = newLayouts;
		for (auto Layouts : CurrentLayout)
		{
			Layouts->SetVisibility(ESlateVisibility::Visible);
		}
	}
}