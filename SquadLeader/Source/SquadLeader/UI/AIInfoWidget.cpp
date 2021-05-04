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
	if(IsValid(StatusMaterial) && MaxHealth != 0.f)
	{
		StatusMaterial->SetScalarParameterValue("Inner_fill", Health/MaxHealth);
	}
}

void UAIInfoWidget::OnMaxHealthChanged(float newValue)
{
	MaxHealth = newValue;
	if (IsValid(StatusMaterial) && MaxHealth != 0.f)
	{
		StatusMaterial->SetScalarParameterValue("Inner_fill", Health / MaxHealth);
	}
}

void UAIInfoWidget::OnShieldChanged(float newValue)
{
	Shield = newValue;
	if (IsValid(StatusMaterial) && MaxShield != 0.f)
	{
		StatusMaterial->SetScalarParameterValue("Outter_fill", Shield / MaxShield);
	}
}

void UAIInfoWidget::OnMaxShieldChanged(float newValue)
{
	MaxShield = newValue;
	if (IsValid(StatusMaterial) && MaxShield != 0.f)
	{
		StatusMaterial->SetScalarParameterValue("Outter_fill", Shield / MaxShield);
	}
}

void UAIInfoWidget::OnMissionChanged(AIBasicState newValue)
{
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

void UAIInfoWidget::OnClassChanged(SoldierClass newValue)
{
	if (AIClass != newValue)
	{
		AIClass = newValue;

		auto colorClass = GetColorFromClass(AIClass);

		StatusMaterial->SetVectorParameterValue("InnerBottomColor", colorClass);

		auto iconClass = GetBrushFromClass(AIClass);

		if(IsValid(iconClass))
			ClassImage->SetBrushFromAsset(iconClass);

		auto iconBackgroundClass = GetBrushBackgroundFromClass(AIClass);

		if(IsValid(iconBackgroundClass))
			ClassImageBackground->SetBrushFromAsset(iconBackgroundClass);
	}
}

USlateBrushAsset* UAIInfoWidget::GetBrushFromMission(AIBasicState missionIn)
{
	switch (missionIn)
	{
	case AIBasicState::Attacking:
	case AIBasicState::Moving:
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

USlateBrushAsset* UAIInfoWidget::GetBrushFromClass(SoldierClass classIn)
{
	switch (classIn)
	{
	case SoldierClass::ASSAULT:
		return AssaultIcon;
		break;
	case SoldierClass::HEAVY:
		return HeavyIcon;
		break;
	case SoldierClass::SUPPORT:
		return SupportIcon;
		break;
	default:
		return nullptr;
		break;
	}
}

USlateBrushAsset* UAIInfoWidget::GetBrushBackgroundFromClass(SoldierClass classIn)
{
	switch (classIn)
	{
	case SoldierClass::SUPPORT:
		return SupportIconBackground;
	case SoldierClass::ASSAULT:
	case SoldierClass::HEAVY:
	default:
		return nullptr;
		break;
	}
}

FLinearColor UAIInfoWidget::GetColorFromClass(SoldierClass classIn)
{
	switch (classIn)
	{
	case SoldierClass::ASSAULT:
		return AssaultColor;
		break;
	case SoldierClass::HEAVY:
		return HeavyColor;
		break;
	case SoldierClass::SUPPORT:
		return SupportColor;
		break;
	default:
		return FLinearColor::White;
		break;
	}
}

void UAIInfoWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (IsValid(StatusMaterialClass))
	{
		StatusMaterial = UMaterialInstanceDynamic::Create(StatusMaterialClass, this);
	}
	if (IsValid(StatusImage) && IsValid(StatusMaterial))
	{
		StatusMaterial->SetScalarParameterValue("Outter_fill", 0.f);
		StatusMaterial->SetScalarParameterValue("Inner_fill", 1.f);
		StatusImage->SetBrushFromMaterial(StatusMaterial);
	}
}
