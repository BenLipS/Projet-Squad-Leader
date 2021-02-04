// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "components/BoxComponent.h"
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


	UPROPERTY(BlueprintReadWrite, Category = "ControlValue")
		int controlValue;
	UPROPERTY(BlueprintReadWrite, Category = "ControlValue")
		int maxControlValue;
	UPROPERTY(BlueprintReadWrite, Category = "ControlValue")
		int presenceValue;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Tempo")
		float tempo1s;
};
