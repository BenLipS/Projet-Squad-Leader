// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Interface/GameEndInterface.h"

#include "Components/TextBlock.h"

#include "GameEndWidget.generated.h"


UCLASS()
class SQUADLEADER_API UGameEndWidget : public USL_UserWidget, public IGameEndInterface
{
	GENERATED_BODY()

public:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn);

	virtual void OnGameEnd(FString EndMessage);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextEndGame;

protected:
	virtual void SynchronizeProperties() override;
};
