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


	maxControlValue = 20;  // maxValue
	controlValueToTake = 10;  // value need to change boolean variables
	
	isTakenBy = ENUM_PlayerTeam::None;

	controlValue = 0;
	presenceTeam1 = 0;
	presenceTeam2 = 0;

	tempo1s = 0;
}

// Called every frame
void AControlArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// create a one second tempo system
	tempo1s += DeltaTime;
	if (tempo1s >= 1) {
		tempo1s = 0;

		if ((presenceTeam1 == 0 && presenceTeam2 > 0) || (presenceTeam2 == 0 && presenceTeam1 > 0)) {
			if (abs(controlValue + presenceTeam1 - presenceTeam2) <= maxControlValue) {
				controlValue += presenceTeam1 - presenceTeam2;
				if (controlValue >= controlValueToTake)
					isTakenBy = ENUM_PlayerTeam::Team1;
				else if (controlValue <= -1 * controlValueToTake)
					isTakenBy = ENUM_PlayerTeam::Team2;
				else
					isTakenBy = ENUM_PlayerTeam::None;
			}
		}
	}
}

void AControlArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (ASoldier* soldier = Cast<ASoldier>(OtherActor); soldier) {
		if (soldier->PlayerTeam == ENUM_PlayerTeam::Team1) {
			presenceTeam1++;
		}
		else if (soldier->PlayerTeam == ENUM_PlayerTeam::Team2) {
			presenceTeam2++;
		}
	}
}

void AControlArea::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (ASoldier* soldier = Cast<ASoldier>(OtherActor); soldier) {
		if (soldier->PlayerTeam == ENUM_PlayerTeam::Team1) {
			if (presenceTeam1 > 0)
				presenceTeam1--;
		}
		else if (soldier->PlayerTeam == ENUM_PlayerTeam::Team2) {
			if (presenceTeam2 > 0)
				presenceTeam2--;
		}
	}
}
