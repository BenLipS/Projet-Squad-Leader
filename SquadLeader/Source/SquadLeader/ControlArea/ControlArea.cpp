// Fill out your copyright notice in the Description page of Project Settings.

#include "ControlArea.h"

// Sets default values
AControlArea::AControlArea()
{
	BoxCollide = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxContactComponent"));
	FVector BoxSize{ 50, 50, 30 };
	BoxCollide->SetBoxExtent(BoxSize, false);
	RootComponent = BoxCollide;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AControlArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AControlArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}