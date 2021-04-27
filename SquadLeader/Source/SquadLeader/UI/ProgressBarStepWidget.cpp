// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressBarStepWidget.h"


#include "Components/HorizontalBoxSlot.h"
#include "Blueprint/WidgetTree.h"

void UProgressBarStepWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	/*Ensure coherent value of the number of bar*/
	ensure(NbSplit >= 1.f);

	UpdateNbSplit();
}

void UProgressBarStepWidget::SetPercent(float InPercent)
{
	if (!(Percent == InPercent))
	{
		Percent = InPercent;
		UpdatePercentDisplay();
	}
}

void UProgressBarStepWidget::SetNbSplit(float InNbSplit)
{
	if (!(NbSplit == InNbSplit))
	{
		NbSplit = InNbSplit;
		UpdateNbSplit();
	}
}

void UProgressBarStepWidget::UpdatePercentDisplay()
{
	int CurrentId = 0;
	UProgressBar* ProgressBarWidget;
	float RemainingSplit = NbSplit;
	float RemainingPercent = Percent * NbSplit;

	auto children = ContainerBox->GetAllChildren();

	while (RemainingSplit > 0.f)
	{
		if (children.IsValidIndex(CurrentId))
		{
			ProgressBarWidget = Cast<UProgressBar>(children[CurrentId]);
			if (IsValid(ProgressBarWidget))
			{
				if (RemainingPercent < 0.f)
				{
					RemainingPercent = 0.f;
				}
				if (RemainingPercent < 1.f && RemainingSplit < 1.f)
				{
					ProgressBarWidget->SetPercent(RemainingPercent / RemainingSplit);
				}
				else
				{
					ProgressBarWidget->SetPercent(RemainingPercent);
				}
			}
		}
		RemainingPercent -= 1.f;
		RemainingSplit -= 1.f;
		CurrentId++;
	}
}

void UProgressBarStepWidget::UpdateNbSplit()
{
	if (IsValid(ContainerBox))
	{
		if (IsValid(ProgressBarModel))
		{
			/*Clear all children*/
			ContainerBox->ClearChildren();

			/*HorizontalBoxSlotValues*/
			FSlateChildSize sizeSlot;
			sizeSlot.SizeRule = ESlateSizeRule::Fill;
			sizeSlot.Value = 1.f;

			/*Creating Widgets*/
			float RemainingSplit = NbSplit;

			while (RemainingSplit > 0.f)
			{
				UProgressBar* newProgressBar = WidgetTree->ConstructWidget<UProgressBar>(ProgressBarModel);
				UHorizontalBoxSlot* ProgressSlot = ContainerBox->AddChildToHorizontalBox(newProgressBar);

				if (RemainingSplit < 1.f)
				{
					sizeSlot.Value = RemainingSplit;
				}

				ProgressSlot->SetSize(sizeSlot);
				ProgressSlot->SetPadding(FMargin(LeftPadding, TopPadding, RightPadding, BottomPadding));

				RemainingSplit -= 1.f;
			}

			/*Initializing percent*/
			UpdatePercentDisplay();
		}
	}
}
