// Fill out your copyright notice in the Description page of Project Settings.

#include "ControlArea.h"
#include "../SquadLeaderGameModeBase.h"
#include "../Soldiers/Soldier.h"


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

	/* Var init*/
	maxControlValue = 20;  // maxValue
	controlValueToTake = maxControlValue / 2;  // value need to change boolean variables

	timeBetweenCalcuation = 0.5;

	if (GetLocalRole() == ROLE_Authority) {
		// add this to the game mode collection
		auto gameMode = static_cast<ASquadLeaderGameModeBase*>(GetWorld()->GetAuthGameMode());
		gameMode->controlAreaCollection.Add(this);

		UpdateTeamData();
	}
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
			if (TeamData.Find(soldier->PlayerTeam)) {
				TeamData[soldier->PlayerTeam].presenceTeam++;

				// initiate the calculation of the control zone value if needed
				if (!timerCalculationControlValue.IsValid())
					GetWorldTimerManager().SetTimer(timerCalculationControlValue, this,
						&AControlArea::calculateControlValue, timeBetweenCalcuation, true, timeBetweenCalcuation);
			}
			else GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("ControlArea : Player of an unknow team"));
		}
	}
}

void AControlArea::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (GetLocalRole() == ROLE_Authority) {  // server only
		if (ASoldier* soldier = Cast<ASoldier>(OtherActor); soldier) {
			if (TeamData.Find(soldier->PlayerTeam)) {
				if (TeamData[soldier->PlayerTeam].presenceTeam > 0)
					TeamData[soldier->PlayerTeam].presenceTeam--;

				// begin the calculation if everybody of this team left and the calculation is not already working
				if (TeamData[soldier->PlayerTeam].presenceTeam == 0)
					if (!timerCalculationControlValue.IsValid())
						GetWorldTimerManager().SetTimer(timerCalculationControlValue, this,
							&AControlArea::calculateControlValue, timeBetweenCalcuation, true, timeBetweenCalcuation);
			}
		}
	}
}

void AControlArea::calculateControlValue()
{
	/*GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : None"));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Max value reach"));*/
	if (GetLocalRole() == ROLE_Authority) {  // only for the server
		
		// check info about the differents teams on point
		int nbTeamOnPoint = 0;
		TSubclassOf<ASoldierTeam> presentTeam;
		for (auto team : TeamData) {
			if (team.Value.presenceTeam > 0) {
				nbTeamOnPoint++;
				presentTeam = team.Key;
			}
		}

		// handling teams information
		if (nbTeamOnPoint == 1) {
			if (TeamData[presentTeam].controlValue < maxControlValue) {
				bool needToDecreaseOtherPresenceFirst = false;
				for (auto& otherTeam : TeamData) {  // reduce the control value in each other team by the number of teamate
					if (otherTeam.Key != presentTeam) {
						if (otherTeam.Value.controlValue >= TeamData[presentTeam].presenceTeam) {
							otherTeam.Value.controlValue -= TeamData[presentTeam].presenceTeam;
							needToDecreaseOtherPresenceFirst = true;
						}
						else {
							otherTeam.Value.controlValue = 0;
						}
						if (isTakenBy == otherTeam.Key && otherTeam.Value.controlValue < controlValueToTake) {  // remove isTakenBy if needed
							isTakenBy = nullptr;
							GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Team control = None"));
						}
					}
				}
				if (!needToDecreaseOtherPresenceFirst) {  // if all other presence value is 0
					TeamData[presentTeam].controlValue += TeamData[presentTeam].presenceTeam;
					if (isTakenBy != presentTeam && TeamData[presentTeam].controlValue >= controlValueToTake) {
						isTakenBy = presentTeam;
						GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Team control =" + presentTeam.GetDefaultObject()->TeamName));
					}
				}
			}
			else { // stop the timer
				GetWorld()->GetTimerManager().ClearTimer(timerCalculationControlValue);
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Max control for " + presentTeam.GetDefaultObject()->TeamName));
			}
		}
		else {  // too much teams on points or nobody
			if (nbTeamOnPoint == 0) {
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Everybody left"));
			}
			// stop the timer
			GetWorld()->GetTimerManager().ClearTimer(timerCalculationControlValue);
		}
	}
}

void AControlArea::UpdateTeamData()
{
	if (GetLocalRole() == ROLE_Authority) {  // only for the server
		auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
		auto teamCollection = gameMode->SoldierTeamCollection;
		for (auto team : teamCollection) {
			if (!TeamData.Find(team))
				TeamData.Add(team);
		}
		/*for (auto team : TeamData) {  // remove element
			if (!teamCollection.Find(team.Key))
				TeamData.Remove(team.Key);
		}*/
	}
}