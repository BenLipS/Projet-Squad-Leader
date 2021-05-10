// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MenuGameInfoInterface.h"

#include "MenuGameInfoDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuGameInfoDelegateInterface : public UMenuGameInfoInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IMenuGameInfoDelegateInterface : public IMenuGameInfoInterface
{
	GENERATED_BODY()

private:
	TArray<IMenuGameInfoInterface*> MenuGameInfoDelegates;

public:
		virtual void OnGameInfoReceived(FString Key, FString Value, FString IP) override;
		virtual void OnGamesInfoCleanOrder() override;

		void AddMenuGameInfoDelegate(IMenuGameInfoInterface* newDelegate);
		void RemoveMenuGameInfoDelegate(IMenuGameInfoInterface* removeDelegate);
};
