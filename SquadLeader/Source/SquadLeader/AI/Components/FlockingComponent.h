// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../AIGeneralController.h"
#include "Components/ActorComponent.h"
#include "FlockingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class SQUADLEADER_API UFlockingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFlockingComponent();

	FVector GetMovementVector() { return MovementVector; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/* The weight of the Alignment vector component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		float AlignementWeight{ 1.f };

	/* The weight of the Cohesion vector component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		float CohesionWeight{ 1.f };

	/* The weight of the Collision vector component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		float SeparationWeight{ 5.f };

	/* The weight of the wall avoidance vector component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		float WallAvoidanceWeight{ 5.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		float ObjectifWeight{ 2.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		float ShootingPositionWeight{ 1.f };

	/* less sacades but more unprecise the greater it gets*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		float MovementVectorScale{ 300.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		int NumberOfRayForWallAvoidance{ 4 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		float RadiusForWallAvoidance{ 300.f };

	UPROPERTY()
		FVector SeparationVector{ 0.f, 0.f, 0.f };

	UPROPERTY()
		FVector WallAvoidanceVector{ 0.f, 0.f, 0.f };

	UPROPERTY()
		FVector CohesionVector{ 0.f, 0.f, 0.f };

	UPROPERTY()
		FVector AlignementVector{ 0.f, 0.f, 0.f };

	UPROPERTY()
		FVector ObjectifVector{ 0.f, 0.f, 0.f };

	UPROPERTY()
		FVector ShootingPositionVector{ 0.f, 0.f, 0.f };

	UPROPERTY()
		FVector MovementVector{ 0.f, 0.f, 0.f };

	UPROPERTY()
		FVector PreviousMovementVector{ 0.f, 0.f, 0.f };

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
	//	float SeparationRange = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		float DefaultNormalizeVectorTolerance = 0.0001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		float BoidPhysicalRadius = 34.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking Behaviour")
		bool DoDrawDebug = false;

	UPROPERTY()
		TArray<AAIGeneralController*> SeenBoids;

	UFUNCTION()
		void ResetVectors();

	UFUNCTION()
		void UpdateNeighbourhood();

	/*Update*/
	UFUNCTION()
		void UpdateCohesionVector();

	UFUNCTION()
		void UpdateAlignementVector();

	UFUNCTION()
		void UpdateSeparationVector();

	UFUNCTION()
		void UpdateObjectifVector();

	UFUNCTION()
		void UpdateShootingPositionVector();

	UFUNCTION()
		void UpdateMovementVector();

	UFUNCTION()
		void UpdateWallAvoidanceVector();

	UFUNCTION()
		void DrawDebug();

	UFUNCTION()
		bool IsFlockingPositionValid();

public:	
	UFUNCTION()
	void UpdateFlockingPosition(float DeltaSeconds);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
