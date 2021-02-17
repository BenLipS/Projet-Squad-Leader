// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxControlArea.h"

// Sets default values
ABoxControlArea::ABoxControlArea()
{
	initCollideElement();

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// used when initialising the control area
void ABoxControlArea::initCollideElement() {
	BoxCollide = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxContactComponent"));
	FVector BoxSize{ 50, 50, 30 };
	BoxCollide->SetBoxExtent(BoxSize, false);
	RootComponent = BoxCollide;
}


// Called when the game starts or when spawned
void ABoxControlArea::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABoxControlArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}