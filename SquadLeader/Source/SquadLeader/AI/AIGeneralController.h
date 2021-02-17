// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptiontypes.h"
#include "AIGeneralController.generated.h"

/**
 * 
 */
UENUM()
enum AIBehavior {
	Attack UMETA(DisplayName = "Attack"),
	Defense UMETA(DisplayName = "Defense"),
};

UCLASS()
class SQUADLEADER_API AAIGeneralController : public AAIController
{
	GENERATED_BODY()

public:
	AAIGeneralController(FObjectInitializer const& object_initializer = FObjectInitializer::Get());

	UFUNCTION()
	void ontargetperception_update_sight(AActor* actor, FAIStimulus stimulus);

	UFUNCTION()
	void onperception_update_sight(const TArray<AActor*>& AArray);
	UFUNCTION()
	void setup_perception_system();
	UFUNCTION()
	void ActorsPerceptionUpdated(const TArray < AActor* >& UpdatedActors);

	UFUNCTION()
	void BeginPlay();
	
	/*Move to a location, the location must be an AActor*/
	UFUNCTION(BlueprintCallable, Category = "SquadLeader")
		EPathFollowingRequestResult::Type MoveToActorLocation();

	/*Move to a location, the location muste be a FVector*/
	UFUNCTION(BlueprintCallable, Category = "SquadLeader")
		EPathFollowingRequestResult::Type MoveToVectorLocation();

	/*Move to the enemy location*/
	UFUNCTION(BlueprintCallable, Category = "SquadLeader")
		EPathFollowingRequestResult::Type MoveToEnemyLocation();


	UFUNCTION(BlueprintCallable, Category = "Shoot")
	void ShootEnemy();

	class UBlackboardComponent* get_blackboard() const;
	
	UFUNCTION(BluePrintCallable, Category = "Comportement")
		virtual void Tick(float DeltaSeconds) override;
private:
	/*Set-up the BehaviorTree at the construction*/
	void setup_BehaviorTree();

	UFUNCTION()
		void Sens();

	UFUNCTION()
		void Think();
	
private:

	/*The behaviorTree that we are running*/
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* m_behaviorTree;
	
	class UBlackboardComponent* blackboard;
	
	class UAISenseConfig_Sight* sight_config;

	UPROPERTY()
	TArray<AActor*> SeenActor;

	UPROPERTY()
		class UBlackboardComponent* m_BlackBoard;

	UPROPERTY()
		TEnumAsByte<AIBehavior> m_behavior;


	UPROPERTY()
		FVector m_destination;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float m_distanceShootAndWalk = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float m_distanceShootAndStop = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
		float m_distancePerception = 5000.f;
};
