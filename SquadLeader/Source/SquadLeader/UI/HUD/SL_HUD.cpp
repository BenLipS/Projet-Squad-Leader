#include "SL_HUD.h"
#include "../SL_UserWidget.h"


void ASL_HUD::InitHUD()
{
	WidgetList.Empty();

	for (TSubclassOf<USL_UserWidget> WidgetClass : WidgetClassList)
	{
		if (WidgetClass)
		{
			USL_UserWidget* newEntry = CreateWidget<USL_UserWidget>(GetWorld(), WidgetClass);
			if (newEntry)
			{
				newEntry->AddToViewport();
				bIsDisplayed = !bIsDisplayed;
				SetHUDDisplay(!bIsDisplayed);
				WidgetList.Add(newEntry);
				newEntry->SetupDelegateToObject(this);
			}
		}
	}

	OnInitHUD();
}

void ASL_HUD::HideHUD()
{
	SetHUDDisplay(false);
}

void ASL_HUD::DisplayHUD()
{
	SetHUDDisplay(true);
}

void ASL_HUD::SetHUDDisplay(bool ShoulBeDisplayed)
{
	if (ShoulBeDisplayed != bIsDisplayed)
	{
		bIsDisplayed = ShoulBeDisplayed;

		float renderOpacity = bIsDisplayed ? 1.f : 0.f;

		for (USL_UserWidget* SL_Widget : WidgetList)
		{
			SL_Widget->SetRenderOpacity(renderOpacity);
		}
	}
}

bool ASL_HUD::IsDisplayed()
{
	return bIsDisplayed;
}

bool ASL_HUD::IsHiden()
{
	return !bIsDisplayed;
}

void ASL_HUD::OnInitHUD()
{
}

void ASL_HUD::BeginPlay()
{
	Super::BeginPlay();
}