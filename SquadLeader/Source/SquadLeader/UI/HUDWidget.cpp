#include "HUDWidget.h"
#include "Kismet/KismetSystemLibrary.h"

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