// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGeneralController.h"
#include "Mission/CaptureMission.h"
#include "AIBasicController.generated.h"

/**
 * 
 */

UCLASS()
class SQUADLEADER_API AAIBasicController : public AAIGeneralController
{
	GENERATED_BODY()
public:
	AAIBasicController();

protected:

	UPROPERTY()
		class AAIBasicManager* m_manager;

	virtual void setup_BehaviorTree() override;

	/*
	* this method is called every Tick/Frame
	*/
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

/////////// Respawn
public:
	virtual FVector GetRespawnPoint() override;

	void SetObjectifLocation(FVector _objLocation) noexcept;

public:

	virtual void Init() override;

	virtual void Die() override;

	virtual void ResetBlackBoard() override;

	void SetManager(AAIBasicManager* _manager) noexcept;

};
