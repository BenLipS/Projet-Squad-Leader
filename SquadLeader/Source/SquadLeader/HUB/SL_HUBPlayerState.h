// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HUBPlayerParam.h"
#include "SL_HUBPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASL_HUBPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void ChangeTeam();
	UFUNCTION(BlueprintCallable)
		void ChangeReadyState();

	UFUNCTION(Client, Reliable)
		void ClientRemoveHUBPlayerParam();
	void ClientRemoveHUBPlayerParam_Implementation();

protected:
	AHUBPlayerParam* LocalHUBPlayerParam;
};
