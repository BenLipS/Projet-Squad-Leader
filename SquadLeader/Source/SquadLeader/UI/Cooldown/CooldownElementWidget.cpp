// Fill out your copyright notice in the Description page of Project Settings.


#include "CooldownElementWidget.h"
#include "Kismet/KismetTextLibrary.h"
#include "Slate/SlateBrushAsset.h"

void UCooldownElementWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsInCooldown)
	{
		TimeRemaining = GetOwningPlayer()->GetWorld()->GetTimerManager().GetTimerRemaining(CooldownTimerHandle);
		if (TextCooldown)
		{
			TextCooldown->SetText(UKismetTextLibrary::Conv_FloatToText(TimeRemaining, ERoundingMode::FromZero, false, true, 1, 324, 1, 1));
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

	PlayAnimation(CD_Ready);

	if (ImageCooldown)
	{
		auto mat = ImageCooldown->GetDynamicMaterial();
		if (mat)
		{
			mat->SetScalarParameterValue("Percentage_fill", 1.f);
			ImageCooldown->SetBrushFromMaterial(mat);
		}
	}

	Icon->SetOpacity(1.f);
}

void UCooldownElementWidget::LaunchPartialCooldown(float Timer, float MaxTimer)
{
	//GetOwningPlayer()->GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, Timer, false);
	GetOwningPlayer()->GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UCooldownElementWidget::OnCooldownEnded, Timer);
	bIsInCooldown = true;
	MaxTimeRemaining = MaxTimer;
	TextCooldown->SetVisibility(ESlateVisibility::HitTestInvisible);
	Icon->SetOpacity(0.5f);
}

void UCooldownElementWidget::LaunchCooldown(float Timer)
{
	LaunchPartialCooldown(Timer, Timer);
	PlayAnimation(CD_Used);
}

ESoldierAbilityInputID UCooldownElementWidget::GetAbilityID()
{
	return AbilityID;
}

void UCooldownElementWidget::SetIcon(USlateBrushAsset* BrushIn)
{
	if (BrushIn)
	{
		Icon->SetBrushFromAsset(BrushIn);
	}
}

void UCooldownElementWidget::InitState(ESoldierAbilityInputID newKey, FString KeyText)
{
	AbilityID = newKey;
	TextKey->SetText(FText::FromString(KeyText));
}