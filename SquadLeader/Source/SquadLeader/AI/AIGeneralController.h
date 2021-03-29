// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../Soldiers/Soldier.h"
#include "../Soldiers/AIs/SoldierAI.h"
#include "Perception/AIPerceptiontypes.h"
#include "Mission/CaptureMission.h"
#include "Mission/DefendMission.h"
#include "Mission/PatrolMission.h"
#include "Misc/TVariant.h"
#include "../Soldiers/Interface/Teamable.h"
#include "AIGeneralController.generated.h"

class UFlockingComponent;

/**
 * This enum contains the different state of the AI 
 */
UENUM()
enum AIBasicState {
	Attacking UMETA(DisplayName = "Attacking"),
	Patroling UMETA(DisplayName = "Patroling"),
	Capturing UMETA(DisplayName = "Capturing"),
	Search UMETA(DisplayName = "Searching"),
	Defend UMETA(DisplayName = "Defending"),
	Moving UMETA(DisplayName = "Moving"),
};

/*
* For a many function we need to send a signal 
* A function can then send if it's a succes, a failure or un progress
*/
UENUM()
enum ResultState {
	Success UMETA(DisplayName = "Success"),
	Failed UMETA(DisplayName = "Failed"),
	InProgress UMETA(DisplayName = "InProgress"),
};


USTRUCT()
struct SQUADLEADER_API Fhome_variant {
	GENERATED_USTRUCT_BODY()

	class AAIGeneralController* m_ai_controller;

	Fhome_variant() = default;
	Fhome_variant(AAIGeneralController* _ai_controller) { m_ai_controller = _ai_controller; }

	auto operator()(UCaptureMission* _mission)const;
	auto operator()(UDefendMission* _mission)const;
	auto operator()(UPatrolMission* _mission)const;
};


UCLASS()
class SQUADLEADER_API AAIGeneralController : public AAIController, public ITeamable
{
	GENERATED_BODY()

public:

	/*
	* Definition of type
	*/
	using type_mission = TVariant<UCaptureMission*, UDefendMission*, UPatrolMission*> ;
	using m_heap_missions = TArray<type_mission>;



	AAIGeneralController(FObjectInitializer const& object_initializer = FObjectInitializer::Get());
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

//////////////// Teamable
protected:
	UPROPERTY(Replicated)
		TSubclassOf<ASoldierTeam> Team = nullptr;  // only server can replicate it
	UPROPERTY()
	FVector ObjectifLocation{ 1000.f, 1000.f, 10.f };
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	TSubclassOf<UFlockingComponent> ClassFlockingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	UFlockingComponent* FlockingComponent;

	UFUNCTION(BlueprintCallable)
		FVector GetObjectifLocation() { return ObjectifLocation + 100; };


	/* For BT Task  */
	UFUNCTION(BlueprintCallable, Category = "Flocking Behaviour")
		EPathFollowingRequestResult::Type FollowFlocking();

	UFUNCTION(BlueprintCallable, Category = "Flocking Behaviour")
		void SetPatrolPoint();

	/*
	* When doing the flocking we'll check if the AI
	* is arrive at his destination
	* if it's the case then we change the state to Patroling
	* else we continue to move
	*/
	UFUNCTION()
		ResultState ArriveAtDestination();


	virtual TSubclassOf<ASoldierTeam> GetTeam() override;
	virtual bool SetTeam(TSubclassOf<ASoldierTeam> _Team) override;


	UFUNCTION()
	void ontargetperception_update_sight(AActor* actor, FAIStimulus stimulus);

	UFUNCTION()
	void onperception_update_sight(const TArray<AActor*>& AArray);
	UFUNCTION()
	void setup_perception_system();
	UFUNCTION()
	void ActorsPerceptionUpdated(const TArray < AActor* >& UpdatedActors);

	/*Run to a location, the location muste be a FVector*/
	UFUNCTION(BlueprintCallable, Category = "SquadLeader")
		EPathFollowingRequestResult::Type RunToVectorLocation(FVector Goal, float AcceptanceRadius);

	UFUNCTION()
	virtual void BeginPlay();

	UFUNCTION()
		void Init();
	
	/*Move to a location, the location must be an AActor*/
	UFUNCTION(BlueprintCallable, Category = "SquadLeader")
		EPathFollowingRequestResult::Type MoveToActorLocation();

	/*Move to a location, the location muste be a FVector*/
	UFUNCTION(BlueprintCallable, Category = "SquadLeader")
		EPathFollowingRequestResult::Type MoveToVectorLocation();

	/*Move to the enemy location*/
	UFUNCTION(BlueprintCallable, Category = "SquadLeader")
		EPathFollowingRequestResult::Type MoveToEnemyLocation();

	/*Move to the last known position of the enemy*/
	UFUNCTION(BlueprintCallable, Category = "SquadLeader")
		EPathFollowingRequestResult::Type MoveToSearchEnemy();

	UPROPERTY(BlueprintReadWrite, Category = "Defense")
	int HalfRadiusPatrol = 1600;

	/*
	* End the search of the enemy
	* rest the state to the old one
	* clear some value of the blackboard
	*/
	UFUNCTION(BlueprintCallable, Category = "SquadLeader")
		ResultState EndTheResearch();

	/*Shoot the enemy we see*/
	UFUNCTION(BlueprintCallable, Category = "Shoot")
	ResultState ShootEnemy();

	UFUNCTION(BlueprintCallable, Category = "Capturing")
		ResultState Capturing();

	class UBlackboardComponent* get_blackboard() const;
	
	/*
	* this method is called every Tick/Frame
	*/
	UFUNCTION(BluePrintCallable, Category = "Comportement")
		virtual void Tick(float DeltaSeconds) override;

	virtual void Die();

	virtual void Respawn();

	virtual void ResetBlackBoard();

	/*
	* Set the state of an AI
	*/
	UFUNCTION()
		void SetState(AIBasicState _state) noexcept;
protected:
	/*Set-up the BehaviorTree at the construction*/
	virtual void setup_BehaviorTree();
private:

	/*
	* The next two method are part of the 
	* SENS-THINK-ACT
	* we only need SENS-THINK because
	* the node of the behavior tree's are the ACT
	*/
	UFUNCTION()
		void Sens();

	/*
	* This method is called during the THINK part
	* The AI will choose to have an Attack Behavior
	* or a more Defensive Behavior
	*/
	UFUNCTION()
		void Think();
	UFUNCTION()
		void Act();

	/*
	* This function will decide wich state the AI shool run
	*/
	void ChooseState();

	/*
	* Will sort the Actor with catch with
	* the perception system
	* And will erase the Actor we can't see anymore
	*/
	UFUNCTION()
		void UpdateSeenSoldier();

	/*
	* After sorting the Actor we see we will choose the enemy to kill
	* if there is one
	*/
	UFUNCTION()
		void FocusEnemy();

	/*
	* Make the AI run if it's possible
	*/
	UFUNCTION()
		void Run(ASoldierAI* _soldier, ASoldier* _soldier_enemy);

	/*
	* Calculate the point where the AI is 
	* in a good range 
	* if the enemy is too close.
	* This method is called only when the AI is in the Attack Behavior
	*/
	UFUNCTION()
		void TooClose();

	/*
	* Calculate the point where the AI is
	* in a good range
	* if the enemy is too far.
	* This method is called only when the AI is in the Attack Behavior
	*/	
	UFUNCTION()
		void TooFar();

	/*
	* Make all in place for the state Attacking
	*/
	UFUNCTION()
		void AttackingState();

	/*
	* Make all in place for the state Patroling
	*/
	UFUNCTION()
		void PatrolingState();

	/*
	* Make all in place for the state Moving
	*/
	UFUNCTION()
		void MovingState();

	UFUNCTION()
		void SearchState();

	UFUNCTION()
		void CapturingState();

protected:
	/*The behaviorTree that we are running*/
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* m_behaviorTree;


	class UBlackboardComponent* blackboard;

	UPROPERTY()
	TArray<ASoldier*> SeenSoldier;

	/*
	* This here represent the state of an AI
	*/
	UPROPERTY()
		int m_state;

	/*
	* this variable is here to help when we loose vision of an enemy
	* this way the AI will return to his old state before he was in the state attack
	*/
	UPROPERTY()
		int m_old_state;

private:
	class UAISenseConfig_Sight* sight_config;

	UPROPERTY()
	FVector m_destination;

	UPROPERTY()
		int tick_value = 0;

	UPROPERTY()
		int max_tick = 2;

public:
	UPROPERTY()
	bool StopCurrentBehavior = false;
	UPROPERTY()
	bool HasStopCurrentBehavior = false;

	UFUNCTION()
		void CheckIfNeedToStopCurrentBehavior();

	TArray<ASoldier*> GetSeenSoldier() { return SeenSoldier; }
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
	* Halfe Angle of shooting cone
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float HalfAngleShoot = 0.05f;

	/*
	* Radius of vision
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
		float m_distancePerception = 5000.f;

	/*
	* Lose Sight Offset
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
		float m_LoseSightOffset = 200.0f;

	/*
	* PeripheralVisionAngleDegrees
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
		float m_PeripheralVisionAngleDegrees = 82.5f;

	/*
	* Stimulus Max Age
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
		float m_MaxAge = 1.f;

	/*
	* AutoSuccessRangeFromLastSeenLocation
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
		float m_AutoSuccessRangeFromLastSeenLocation = 100.0f;

	/*
	* Detect Enemies
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
		bool m_DetectEnemies = true;

	/*
	* Detect Friendlies
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
		bool m_DetectFriendlies = true;

	/*
	* Detect Neutrals
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
		bool m_DetectNeutrals = true;

/////////// Respawn
public:
	UFUNCTION()
	virtual FVector GetRespawnPoint() { return FVector(0.f, 0.f, 1500.f); }  // function overide in in each controller

public:	//Mission

	/*
	* Set a mission of type T
	*/
	template<class T>
	void SetMission(T _mission);
	
	/*
	* Return the current mission that the NPC is running
	*/
	auto GetMission();

	/*
	* Set a control area as the destination for the AI
	*/
	UFUNCTION()
		void SetControlAreaBB(AControlArea* _controlArea);

	/*
	* Set the Objectif Location of the AI
	* And it'll set the state to moving
	*/
	UFUNCTION(BlueprintCallable)
		void SetObjectifLocation(FVector _location) noexcept;

protected:

	/*
	* variables for the mission system
	* represent a heap of mission
	*/

	//represent on mission
	type_mission m_mission_type;

	//represent a heap of missions
	m_heap_missions m_missions;

	//the variant that we are using
	Fhome_variant m_variant;

	bool m_mission_changed = false;
};