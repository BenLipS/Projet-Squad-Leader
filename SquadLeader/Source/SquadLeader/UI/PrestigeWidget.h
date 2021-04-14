#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Engine/Canvas.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interface/PlayerPrestigeInterface.h"
#include "Interface/PlayerPrestigeDelegateInterface.h"
#include "PrestigeWidget.generated.h"

UCLASS()
class SQUADLEADER_API UPrestigeWidget : public USL_UserWidget, public IPlayerPrestigeInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* ProgressBarPrestige;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextPrestige;

	float Prestige = 0.f;
	float PrestigeLevelUp = 0.f;

public:
	UPrestigeWidget(const FObjectInitializer& ObjectInitializer);

	void SetupDelegateToObject(UObject* ObjectIn) override;

	//-----IPlayerPrestigeInterface-----
	void OnPlayerPrestigeChanged(float newValue) override;
	void OnPlayerPrestigeLevelUpChanged(float newValue) override;
};