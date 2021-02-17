// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ControlArea.h"
#include "components/BoxComponent.h"
#include "BoxControlArea.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ABoxControlArea : public AControlArea
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ABoxControlArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/** Zone Collide */
	virtual void initCollideElement();
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "DetectionArea")
		class UBoxComponent* BoxCollide;
};
