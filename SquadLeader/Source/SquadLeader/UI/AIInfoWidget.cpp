// Fill out your copyright notice in the Description page of Project Settings.


#include "AIInfoWidget.h"
#include "Slate/SlateBrushAsset.h"

UAIInfoWidget::UAIInfoWidget(const FObjectInitializer& ObjectInitializer) : USL_UserWidget(ObjectInitializer)
{
	OnMissionChanged(Mission);
}

void UAIInfoWidget::OnHealthChanged(float newValue)
{
	Health = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
	TextAI->SetText(FText::FromString(FString::SanitizeFloat(Health, 0)));
}

void UAIInfoWidget::OnMaxHealthChanged(float newValue)
{
	MaxHealth = newValue;
	ProgressBarHP->SetPercent(Health / MaxHealth);
}

void UAIInfoWidget::OnShieldChanged(float newValue)
{
	Shield = newValue;
	ProgressBarShield->SetPercent(Shield / MaxShield);
}

void UAIInfoWidget::OnMaxShieldChanged(float newValue)
{
	MaxShield = newValue;
	ProgressBarShield->SetPercent(Shield / MaxShield);
}

void UAIInfoWidget::OnMissionChanged(AIBasicState newValue)
{
	//ImageMission->SetBrush();
	//ImageMission->getBrus
	if (Mission != newValue)
	{
		Mission = newValue;
		auto brushMission = GetBrushFromMission(Mission);
		if (IsValid(brushMission))
		{
			ImageMission->SetBrushFromAsset(brushMission);
		}
	}
}

USlateBrushAsset* UAIInfoWidget::GetBrushFromMission(AIBasicState missionIn)
{
	switch (missionIn)
	{
	case AIBasicState::Attacking:
		return AttackingImage;
		break;
	case AIBasicState::Formation:
		return FormationImage;
		break;
	default:
		return DefaultImage;
		break;
	}
}
