// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptiontypes.h"
#include "AIGeneralController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AAIGeneralController : public AAIController
{
	GENERATED_BODY()

public:
	AAIGeneralController(FObjectInitializer const& object_initializer = FObjectInitializer::Get());

	class UAISenseConfig_Sight* sight_config;

	UFUNCTION()
	void on_update_sight2(AActor* actor, FAIStimulus const stimulus);

	UFUNCTION()
	void on_update_sight(const TArray<AActor*>& AArray);

	void setup_perception_system();

	UFUNCTION()
	void BeginPlay();

	UFUNCTION(BlueprintCallable, Category = "Shoot")
	void ShootEnemy();


private:
	/*Set-up the BehaviorTree at the construction*/
	void setup_BehaviorTree();

private:

	/*The behaviorTree that we are running*/
	UPROPERTY()
		class UBehaviorTree* m_behaviorTree;

};
