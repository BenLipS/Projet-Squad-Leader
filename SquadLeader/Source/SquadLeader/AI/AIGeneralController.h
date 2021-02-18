// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../Soldiers/Soldier.h"
#include "../Soldiers/AIs/SoldierAI.h"
#include "Perception/AIPerceptiontypes.h"
#include "AIGeneralController.generated.h"

/**
 * This enum contains the different behavior the AI can have
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

	/*Shoot the enemy we see*/
	UFUNCTION(BlueprintCallable, Category = "Shoot")
	void ShootEnemy();

	class UBlackboardComponent* get_blackboard() const;
	
	/*
	* this method is called every Tick/Frame
	*/
	UFUNCTION(BluePrintCallable, Category = "Comportement")
		virtual void Tick(float DeltaSeconds) override;
private:
	/*Set-up the BehaviorTree at the construction*/
	void setup_BehaviorTree();

	/*
	* The next two method are part of the 
	* SENS-THINK-ACT
	* we only need SENS-THINK because
	* the node of the behavior tree's are the ACT
	*/
	UFUNCTION()
		void Sens();
	UFUNCTION()
		void Think();

	/*
	* This method is called during the THINK part
	* The AI will choose to have an Attack Behavior
	* or a more Defensive Behavior
	*/
	UFUNCTION()
		void ChooseBehavior();

	/*
	* Will sort the Actor with catch with
	* the perception system
	* And will erase the Actor we can't see anymore
	*/
	UFUNCTION()
		void SortActorPerception();

	/*
	* After sorting the Actor we see we will choose the enemy to kill
	* if there is one
	*/
	UFUNCTION()
		void SearchEnemy();

	/*
	* Make the AI run if it's possible
	*/
	UFUNCTION()
		void Run(ASoldierAI* _soldier, ASoldier* _soldier_enemy);
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

	/*
	* The distance from where we can walk and shoot the enemy
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float m_distanceShootAndWalk = 3000.f;

	/*
	* The distance fron where we are at a good distance for shooting 
	* and the AI need to sptop
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float m_distanceShootAndStop = 1000.f;

	/*
	* Radius of vision
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
		float m_distancePerception = 5000.f;
};
