// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatInfoInterface.h"

#include "StatInfoDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStatInfoDelegateInterface : public UStatInfoInterface
{
	GENERATED_BODY()
};


class SQUADLEADER_API IStatInfoDelegateInterface : public IStatInfoInterface
{
	GENERATED_BODY()
private:
	TArray<IStatInfoInterface*> StatInfoDelegates;

public:
	virtual void OnStatInfoReceived(FString Key, FString Value) override;
	virtual void OnStatsInfoReceived(TMap<FString, FString> statsIn) override;
	virtual void OnStatsInfoCleanOrder() override;

	void AddStatInfoDelegate(IStatInfoInterface* newDelegate);
	void RemoveStatInfoDelegate(IStatInfoInterface* removeDelegate);
};
