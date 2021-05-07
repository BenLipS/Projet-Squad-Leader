// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MenuListInfo.h"
#include "../Interface/MenuGameInfoDelegateInterface.h"

#include "MenuListGame.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMenuListGame : public UMenuListInfo,
	public IMenuGameInfoInterface
{
	GENERATED_BODY()

//-----UMenuItemWidget-----
protected:
	virtual void OnItemAddedToLayout_Implementation() override;
	
//-----IMenuGameInfoInterface-----
public:
	virtual void OnGameInfoReceived(FString Key, FString Value, FString IP) override;
	virtual void OnGamesInfoCleanOrder() override;
};
