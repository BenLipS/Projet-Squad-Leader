// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HUBPlayerParam.h"
#include "SLHUBPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASLHUBPlayerController : public APlayerController
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
