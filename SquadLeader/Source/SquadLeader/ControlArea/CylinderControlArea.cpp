// Fill out your copyright notice in the Description page of Project Settings.


#include "CylinderControlArea.h"

// Sets default values
ACylinderControlArea::ACylinderControlArea()
{
	initCollideElement();

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// used when initialising the control area
void ACylinderControlArea::initCollideElement() {
	CylinderCollide = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleContactComponent"));
	CylinderCollide->SetCapsuleRadius(30, false);
	CylinderCollide->SetCapsuleHalfHeight(40, false);
	RootComponent = CylinderCollide;
}


// Called when the game starts or when spawned
void ACylinderControlArea::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACylinderControlArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}