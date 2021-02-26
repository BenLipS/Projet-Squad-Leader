#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "HUDWidget.generated.h"

UCLASS()
class SQUADLEADER_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

//////////////// Setters - called from a controller
public:
	UFUNCTION(BlueprintCallable)
	void SetRespawnCountdown(const float _TimeRemaining);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxHealth(float const _MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealth(float const _Health);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxShield(float const _MaxShield);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetShield(float const _Shield);

//////////////// Respawn
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextRespawnRemainTime;

	float RespawnRemainTime = 0.f;

	UFUNCTION()
	void UpdateTextRespawnRemainTime();

//////////////// Timers
	FTimerHandle RespawnTimer;
};