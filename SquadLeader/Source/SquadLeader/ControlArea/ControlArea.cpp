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

	if (auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); gameMode) {
		// add this to the game mode collection
		gameMode->ControlAreaManager.GetDefaultObject()->AddControlArea(this);

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
			if (TeamData.Contains(soldier->GetTeam())) {
				TeamData[soldier->GetTeam()]->presenceTeam++;

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
			if (TeamData.Contains(soldier->GetTeam())) {
				if (TeamData[soldier->GetTeam()]->presenceTeam > 0) {
					TeamData[soldier->GetTeam()]->presenceTeam--;
				}

				// begin the calculation if everybody of this team left and the calculation is not already working
				if (TeamData[soldier->GetTeam()]->presenceTeam == 0) {
					if (!timerCalculationControlValue.IsValid())
						GetWorldTimerManager().SetTimer(timerCalculationControlValue, this,
							&AControlArea::calculateControlValue, timeBetweenCalcuation, true, timeBetweenCalcuation);
				}
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
			if (team.Value->presenceTeam > 0) {
				nbTeamOnPoint++;
				presentTeam = team.Key;
			}
		}

		// handling teams information
		if (nbTeamOnPoint == 1) {
			if (TeamData[presentTeam]->controlValue < maxControlValue) {
				bool needToDecreaseOtherPresenceFirst = false;
				for (auto& otherTeam : TeamData) {  // reduce the control value in each other team by the number of teamate
					if (otherTeam.Key != presentTeam) {
						if (otherTeam.Value->controlValue >= TeamData[presentTeam]->presenceTeam) {
							otherTeam.Value->controlValue -= TeamData[presentTeam]->presenceTeam;
							needToDecreaseOtherPresenceFirst = true;
						}
						else {
							otherTeam.Value->controlValue = 0;
						}
						if (isTakenBy == otherTeam.Key && otherTeam.Value->controlValue < controlValueToTake) {  // remove isTakenBy if needed
							// notify here the changement if needed
							isTakenBy = nullptr;
							otherTeam.Value->ChangeSpawnState(false);
							GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Team control = None"));
						}
					}
				}
				if (!needToDecreaseOtherPresenceFirst) {  // if all other presence value is 0
					if (TeamData[presentTeam]->controlValue + TeamData[presentTeam]->presenceTeam < maxControlValue)
						TeamData[presentTeam]->controlValue += TeamData[presentTeam]->presenceTeam;
					else
						TeamData[presentTeam]->controlValue = maxControlValue;

					if (isTakenBy != presentTeam && TeamData[presentTeam]->controlValue >= controlValueToTake) {  // take control of the point
						isTakenBy = presentTeam;
						TeamData[presentTeam]->ChangeSpawnState(true);
						// notify here the changement if needed
						GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Team control =" + presentTeam.GetDefaultObject()->TeamName));

						// check the victory condition
						if (auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); gameMode) {
							gameMode->CheckControlAreaVictoryCondition();
						}
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
			}
			// stop the timer
			GetWorld()->GetTimerManager().ClearTimer(timerCalculationControlValue);
		}
	}
}

void AControlArea::UpdateTeamData()
{
	if (auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); gameMode) {  // only for the server
		auto teamCollection = gameMode->SoldierTeamCollection;

		TArray<TSubclassOf<ASoldierTeam>> keyToRemove;  // remove element
		for (auto team : TeamData) {
			if (!teamCollection.Contains(team.Key)) {
				keyToRemove.Add(team.Key);
				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea Update : Unknown team removed : " + team.Key.GetDefaultObject()->TeamName));
			}
		}
		for (auto key : keyToRemove) {
			TeamData.Remove(key);
		}

		for (auto team : teamCollection) {  // add element
			if (!TeamData.Contains(team)) {
				TeamData.Add(team, NewObject<AControlAreaTeamStat>());
				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea Update : Unknown team added : " + team.GetDefaultObject()->TeamName));
			}
		}

		for (auto& team : TeamData) {  // update spawn state
			if (team.Value) {
				team.Value->ChangeSpawnState(isTakenBy == team.Key);
				team.Value->ChangeSpawnTeam(team.Key);
			}
		}
	}
}