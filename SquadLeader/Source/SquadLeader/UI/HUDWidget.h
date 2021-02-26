#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

UCLASS()
class SQUADLEADER_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetRespawnCountdown(const float _TimeRemaining);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxHealth(float const _MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealth(float const _Health);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxShield(float const _MaxShield);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetShield(float const _Shield);
};