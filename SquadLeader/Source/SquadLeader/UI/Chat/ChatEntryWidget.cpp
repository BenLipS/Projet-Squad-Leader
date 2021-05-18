// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatEntryWidget.h"
#include "ChatWidget.h"

void UChatEntryWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == StayAnimation)
	{
		bForceDisplay = false;
		TryPlayFadeAnimation();
	}
}

void UChatEntryWidget::SetEntryText(FString newText)
{
	if (IsValid(TextContainer))
	{
		TextContainer->SetText(FText::FromString(newText));
	}
}

void UChatEntryWidget::SetStayTime(float timeIn)
{
	if (timeIn > 0.f)
	{
		PlayAnimationForward(StayAnimation, 1.f/timeIn);
		bForceDisplay = true;
	}
}

void UChatEntryWidget::TryPlayFadeAnimation()
{
	if (!bForceDisplay)
	{
		if (IsValid(ChatOwner) && !ChatOwner->IsChatOpen())
		{
			if (!IsAnimationPlaying(StayAnimation))
			{
				PlayAnimation(FadeAnimation);
			}
		}
	}
}

void UChatEntryWidget::CancelFadeAnimation()
{
	if (!bForceDisplay)
	{
		PlayAnimationReverse(FadeAnimation, 20.f);
	}
}
