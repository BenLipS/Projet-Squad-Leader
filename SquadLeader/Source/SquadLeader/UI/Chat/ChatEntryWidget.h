// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"

#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

#include "ChatEntryWidget.generated.h"

class UChatWidget;

UCLASS()
class SQUADLEADER_API UChatEntryWidget : public USL_UserWidget
{
	friend class UChatWidget;
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeAnimation;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* StayAnimation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bForceDisplay;

	UChatWidget* ChatOwner;

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation);

public:
	void SetEntryText(FString newText);

	void SetStayTime(float timeIn);

	void TryPlayFadeAnimation();

	void CancelFadeAnimation();

};
