// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MenuListInfo.h"
#include "../../../Interface/StatInfoInterface.h"

#include "MenuListInfoStat.generated.h"

UCLASS()
class SQUADLEADER_API UMenuListInfoStat : public UMenuListInfo, public IStatInfoInterface
{
	GENERATED_BODY()

//-----UMenuItemWidget-----
protected:
	virtual void OnItemAddedToLayout_Implementation() override;

//-----IStatInfoInterface-----
public:
	virtual void OnStatInfoReceived(FString Key, FString Value) override;
	virtual void OnStatsInfoReceived(TMap<FString, FString> statsIn) override;
	virtual void OnStatsInfoCleanOrder() override;
};