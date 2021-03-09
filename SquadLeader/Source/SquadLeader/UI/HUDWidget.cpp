#include "HUDWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SquadLeader/Soldiers/Players/SoldierPlayerController.h"
#include "SquadLeader/Soldiers/Soldier.h"

void UHUDWidget::SetRespawnCountdown(const float _TimeRemaining)
{
	RespawnRemainTime = _TimeRemaining;
	TextRespawnRemainTime->SetText(FText::FromString(FString::SanitizeFloat(RespawnRemainTime)));
	RespawnTimer = UKismetSystemLibrary::K2_SetTimer(this, "UpdateTextRespawnRemainTime", 1.f, false);
}

void UHUDWidget::UpdateTextRespawnRemainTime()
{
	if (RespawnRemainTime > 0.0f)
	{
		--RespawnRemainTime;
		TextRespawnRemainTime->SetText(FText::FromString(FString::SanitizeFloat((int32)RespawnRemainTime)));
		RespawnTimer = UKismetSystemLibrary::K2_SetTimer(this, "UpdateTextRespawnRemainTime", 1.f, false);
		// TODO: Remove the digit after coma ?
	}
}

void UHUDWidget::SetMaxHealth(float const _MaxHealth)
{
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwningPlayer()); PC)
	{
		if (ASoldier* soldier = Cast<ASoldier>(PC->GetPawn()); soldier)
			ProgressBarHP->SetPercent(soldier->GetHealth() / _MaxHealth);
	}
}

void UHUDWidget::SetHealth(float const _Health)
{
	TextHP->SetText(FText::FromString(FString::SanitizeFloat(_Health)));

	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwningPlayer()); PC)
	{
		if (ASoldier* soldier = Cast<ASoldier>(PC->GetPawn()); soldier)
			ProgressBarHP->SetPercent(_Health / soldier->GetMaxHealth());
	}
}

void UHUDWidget::SetMaxShield(float const _MaxShield)
{
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwningPlayer()); PC)
	{
		if (ASoldier* soldier = Cast<ASoldier>(PC->GetPawn()); soldier)
			ProgressBarShield->SetPercent(soldier->GetShield() / _MaxShield);
	}
}

void UHUDWidget::SetShield(float const _Shield)
{
	TextShield->SetText(FText::FromString(FString::SanitizeFloat(_Shield)));

	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwningPlayer()); PC)
	{
		if (ASoldier* soldier = Cast<ASoldier>(PC->GetPawn()); soldier)
			ProgressBarShield->SetPercent(_Shield / soldier->GetMaxShield());
	}
}