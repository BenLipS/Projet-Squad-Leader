// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGeneralController.h"
#include "AIBasicController.generated.h"

/**
 * 
 */

UCLASS()
class SQUADLEADER_API AAIBasicController : public AAIGeneralController
{
	GENERATED_BODY()

	AAIBasicController();

protected:

	virtual void setup_BehaviorTree() override;

	/*
	* this method is called every Tick/Frame
	*/
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	/* The weight of the Alignment vector component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	float AlignementWeight;

	/* The weight of the Cohesion vector component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	float CohesionWeight;

	/* The weight of the Collision vector component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	float SeparationWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	float ObjectifWeight;

	/* less sacades but more unprecise the greater it gets*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	float MovementVectorScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	FVector SeparationVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	FVector CohesionVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	FVector AlignementVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	FVector ObjectifVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	FVector MovementVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	FVector ObjectifLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	float DefaultNormalizeVectorTolerance = 0.0001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	float BoidPhysicalRadius = 34.0f;

	UPROPERTY()
	TArray<AAIBasicController*> SeenBoids;

	UFUNCTION()
	void ResetVectors();

	UFUNCTION()
	void UpdateNeighbourhood();

	/*Update and normalize vectors*/
	UFUNCTION()
	void UpdateCohesionVector();

	UFUNCTION()
	void UpdateAlignementVector();

	UFUNCTION()
	void UpdateSeparationVector();

	UFUNCTION()
	void UpdateObjectifVector();

	UFUNCTION()
	void UpdateMovementVector();

	UFUNCTION()
	void DrawDebug();

	UFUNCTION()
	void UpdateFlockingPosition(float DeltaSeconds);

public:
	/* For BT Task  */
	UFUNCTION(BlueprintCallable, Category = "Flocking Behaviour")
	EPathFollowingRequestResult::Type FollowFlocking();

	UFUNCTION(BlueprintCallable, Category = "Flocking Behaviour")
	FVector GetObjectifLocation() { return ObjectifLocation; };
/////////// Respawn
public:
	virtual FVector GetRespawnPoint() override;

	void SetObjectifLocation(FVector _objLocation) noexcept;

public://Mission
	UFUNCTION()
	void UpdateMission();

	virtual void Die() const override;

};
