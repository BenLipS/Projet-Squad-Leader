// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGeneralController.h"
#include "AISquadController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AAISquadController : public AAIGeneralController
{
	GENERATED_BODY()
	
protected:
	virtual void setup_BehaviorTree() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation Behaviour")
	float RuningDistanceForFormation = 100.f;
/////////// Respawn
public:
	AAISquadController();

	virtual FVector GetRespawnPoint() override;

public:
	/* For BT Task  */
	UFUNCTION(BlueprintCallable, Category = "Formation Behaviour")
	EPathFollowingRequestResult::Type FollowFormation();

};
