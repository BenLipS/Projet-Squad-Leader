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
					CurrentLayout = MenuLayout;
				}
			}
		}
	}
}

void UMenuWidget::SetCurrentLayout(FString newlayoutID)
{
	if (int32 index = MenuLayouts.FindLastByPredicate([&newlayoutID](UMenuLayoutWidget* layout) {return layout->GetLayoutID() == newlayoutID; }); index != INDEX_NONE)
	{
		if(CurrentLayout && !IsDesignTime())
			CurrentLayout->SetVisibility(ESlateVisibility::Collapsed);

		CurrentLayout = MenuLayouts[index];
		CurrentLayout->SetVisibility(ESlateVisibility::Visible);
	}
}