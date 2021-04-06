// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ControlArea.h"
#include "components/CapsuleComponent.h"
#include "CylinderControlArea.generated.h"

/** Only usable if bullet and IA don't bug near the collide cylinder
 * 
 */
UCLASS()
class SQUADLEADER_API ACylinderControlArea : public AControlArea
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ACylinderControlArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/** Zone Collide */
	virtual void initCollideElement();
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "DetectionArea")
		class UCapsuleComponent* CylinderCollide;
};