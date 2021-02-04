// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "components/BoxComponent.h"
#include "../Characters/Soldiers/Soldier.h"
#include "ControlArea.generated.h"

UCLASS()
class SQUADLEADER_API AControlArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AControlArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/** Box Collide */
	UPROPERTY(VisibleDefaultsOnly, Category = "DetectionArea")
		class UBoxComponent* BoxCollide;

	/** Control value variables */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ControlValue")
		int controlValue;
	UPROPERTY(BlueprintReadWrite, Category = "ControlValue")
		int maxControlValue;
	UPROPERTY(BlueprintReadWrite, Category = "ControlValue")
		int controlValueToTake;
	UPROPERTY(BlueprintReadWrite, Category = "ControlValue")
		int presenceTeam1;
	UPROPERTY(BlueprintReadWrite, Category = "ControlValue")
		int presenceTeam2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "IsTaken")
		ENUM_PlayerTeam isTakenBy;

	/**
	 *	Event when this actor overlaps another actor, for example a player walking into a trigger.
	 *	For events when objects have a blocking collision, for example a player hitting a wall, see 'Hit' events.
	 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/**
	 *	Event when an actor no longer overlaps another actor, and they have separated.
	 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	 */
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Tempo")
		float tempo1s;
};
