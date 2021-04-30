// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleActor.generated.h"

UCLASS()
class SQUADLEADER_API AObstacleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObstacleActor();
	~AObstacleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitObstacleActor();

	void DestroyObstacle();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY()
	uint8 TickNumber = 0;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team ID")
		uint8 Team = 1;
};
