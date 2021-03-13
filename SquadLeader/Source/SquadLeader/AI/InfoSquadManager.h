// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "InfoSquadManager.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FArraySoldierAIChanged, TArray<class ASoldierAI*>, newValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFloatValueChanged, float, newValue);


UCLASS()
class SQUADLEADER_API AInfoSquadManager : public AInfo
{
	GENERATED_BODY()
public:
	FArraySoldierAIChanged OnSquadAIChanged;

public:
	AInfoSquadManager();

protected:
	void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepSquadAIChanged)
	TArray<class ASoldierAI*> SquadAI;

public:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION()
	void OnSquadManagerChange(TArray<class AAISquadController*> controllerSquad);

	UFUNCTION()
	void OnRepSquadAIChanged();
};
