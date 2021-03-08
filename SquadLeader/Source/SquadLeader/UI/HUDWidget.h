#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "HUDWidget.generated.h"

UCLASS()
class SQUADLEADER_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

//////////////// Setters - called from a controller
public:
	UFUNCTION(BlueprintCallable)
	void SetRespawnCountdown(const float _TimeRemaining);

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float const _MaxHealth);

	UFUNCTION(BlueprintCallable)
	void SetHealth(float const _Health);

	UFUNCTION(BlueprintCallable)
	void SetMaxShield(float const _MaxShield);

	UFUNCTION(BlueprintCallable)
	void SetShield(float const _Shield);

//////////////// Respawn
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextRespawnRemainTime;

	float RespawnRemainTime = 0.f;

	UFUNCTION()
	void UpdateTextRespawnRemainTime();

//////////////// HP
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextHP;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* ProgressBarHP;

//////////////// Shield
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextShield;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* ProgressBarShield;

//////////////// Timers
	FTimerHandle RespawnTimer;
};