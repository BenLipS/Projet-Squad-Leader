// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGeneralController.h"
#include "AISquadController.generated.h"

class AAISquadManager;

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AAISquadController : public AAIGeneralController
{
	GENERATED_BODY()
	
protected:
	virtual void setup_BehaviorTree() override;

	bool GetValidFormationPos();

public:

	UPROPERTY()
		bool HysteresisDoRunningFormation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation Behaviour")
		float HysteresisRunningDistanceForFormation = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation Behaviour")
		float StopHysteresisRunningDistanceForFormation = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation Behaviour")
	float HysteresisDistanceForFormation = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation Behaviour")
	float StopHysteresisDistanceForFormation = 100.f;

/////////// Respawn
public:
	AAISquadController();

	virtual FVector GetRespawnPoint() override;

	virtual void BeginPlay() override;

	/*not used*/
	virtual void Init() override;

	UPROPERTY()
	AAISquadManager* SquadManager;

	UPROPERTY()
		bool RunToFormation = false;
public:
	/* For BT Task  */
	UFUNCTION(BlueprintCallable, Category = "Formation Behaviour")
	void FollowFormation();

	virtual void Die() override;

	virtual void ResetBlackBoard() override;

	void FormationState();

	UFUNCTION()
		void UpdateFormation(const FVector _position);

	UFUNCTION()
		void SetUpMission(bool hasOrder, bool isInFormation, FVector _Location);

protected:
	
	virtual void ChooseState() override;
};
