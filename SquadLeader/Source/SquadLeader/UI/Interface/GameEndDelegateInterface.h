// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameEndInterface.h"

#include "GameEndDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameEndDelegateInterface : public UGameEndInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IGameEndDelegateInterface : public IGameEndInterface
{
	GENERATED_BODY()

private:
	TArray<IGameEndInterface*> GameEndDelegates;

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnGameEnd(FString EndMessage) override;

	void AddGameEndDelegate(IGameEndInterface* newDelegate);
	void RemoveGameEndDelegate(IGameEndInterface* removeDelegate);
};
