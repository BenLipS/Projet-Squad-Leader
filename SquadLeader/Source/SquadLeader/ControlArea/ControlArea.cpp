// Fill out your copyright notice in the Description page of Project Settings.

#include "ControlArea.h"

// Sets default values
AControlArea::AControlArea()
{
	initCollideElement();

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// used when initialising the control area
void AControlArea::initCollideElement() {
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

	timeBetweenCalcuation = 0.5;
}

// Called every frame
void AControlArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AControlArea::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AControlArea, isTakenBy, COND_None, REPNOTIFY_Always);
}


void AControlArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (GetLocalRole() == ROLE_Authority) 
	{  // server only
		if (ASoldier* soldier = Cast<ASoldier>(OtherActor); soldier) {
			if (soldier->PlayerTeam == ENUM_PlayerTeam::Team1) {
				presenceTeam1++;
			}
			else if (soldier->PlayerTeam == ENUM_PlayerTeam::Team2) {
				presenceTeam2++;
			}
			else GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Player of an unknow team"));

			// initiate the calculation of the control zone value if needed
			if (!timerCalculationControlValue.IsValid())
				GetWorldTimerManager().SetTimer(timerCalculationControlValue, this,
					&AControlArea::calculateControlValue, timeBetweenCalcuation, true, timeBetweenCalcuation);
		}
	}
}

void AControlArea::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (GetLocalRole() == ROLE_Authority) {  // server only
		if (ASoldier* soldier = Cast<ASoldier>(OtherActor); soldier) {
			if (soldier->PlayerTeam == ENUM_PlayerTeam::Team1) {
				if (presenceTeam1 > 0)
					presenceTeam1--;
			}
			else if (soldier->PlayerTeam == ENUM_PlayerTeam::Team2) {
				if (presenceTeam2 > 0)
					presenceTeam2--;
			}

			// end the calculation if everybody left
			if (presenceTeam1 == 0 && presenceTeam2 == 0)
				GetWorld()->GetTimerManager().ClearTimer(timerCalculationControlValue);
		}
	}
}

void AControlArea::calculateControlValue()
{
	if (GetLocalRole() == ROLE_Authority) {  // only call by the server
		if ((presenceTeam1 == 0 && presenceTeam2 > 0) || (presenceTeam2 == 0 && presenceTeam1 > 0)) {
			if (abs(controlValue + presenceTeam1 - presenceTeam2) <= maxControlValue) {
				controlValue += presenceTeam1 - presenceTeam2;
				if (controlValue >= controlValueToTake){
					if (isTakenBy != ENUM_PlayerTeam::Team1) {
						isTakenBy = ENUM_PlayerTeam::Team1;
						GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Control : Team1"));
					}
				}
				else if (controlValue <= -1 * controlValueToTake) {
					if (isTakenBy != ENUM_PlayerTeam::Team2) {
						isTakenBy = ENUM_PlayerTeam::Team2;
						GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Control : Team2"));
					}
				}
				else if (isTakenBy != ENUM_PlayerTeam::None){
					isTakenBy = ENUM_PlayerTeam::None;
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Control : None"));
				}
			}
			else { // if the max value is reached
				GetWorld()->GetTimerManager().ClearTimer(timerCalculationControlValue);
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Max value reach"));
			}
		}
	}
}
