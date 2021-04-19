// Fill out your copyright notice in the Description page of Project Settings.


#include "CooldownElementWidget.h"

void UCooldownElementWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsInCooldown)
	{
		TimeRemaining = GetOwningPlayer()->GetWorld()->GetTimerManager().GetTimerRemaining(CooldownTimerHandle);
		if (TextCooldown)
		{
			TextCooldown->SetText(FText::FromString(FString::SanitizeFloat(TimeRemaining, 0)));
		}
		if (ImageCooldown)
		{
			auto mat = ImageCooldown->GetDynamicMaterial();
			if (mat)
			{
				mat->SetScalarParameterValue("Percentage_fill", (MaxTimeRemaining - TimeRemaining) / MaxTimeRemaining);
				ImageCooldown->SetBrushFromMaterial(mat);
			}
		}

	}
}

void UCooldownElementWidget::OnCooldownEnded()
{
	bIsInCooldown = false;
	TextCooldown->SetVisibility(ESlateVisibility::Collapsed);
	TimeRemaining = 0.f;
	if (ImageCooldown)
	{
		auto mat = ImageCooldown->GetDynamicMaterial();
		if (mat)
		{
			mat->SetScalarParameterValue("Percentage_fill", 0.f);
			ImageCooldown->SetBrushFromMaterial(mat);
		}
	}
}

void UCooldownElementWidget::LaunchPartialCooldown(float Timer, float MaxTimer)
{
	//GetOwningPlayer()->GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, Timer, false);
	GetOwningPlayer()->GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UCooldownElementWidget::OnCooldownEnded, Timer);
	bIsInCooldown = true;
	TextCooldown->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UCooldownElementWidget::LaunchCooldown(float Timer)
{
	LaunchPartialCooldown(Timer, Timer);
}

ESoldierAbilityInputID UCooldownElementWidget::GetAbilityID()
{
	return AbilityID;
}

void UCooldownElementWidget::InitState(ESoldierAbilityInputID newKey)
{
	AbilityID = newKey;
	TextKey->SetText(FText::FromString(""));
}