// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierTeam.h"
#include "Soldier.h"
#include "../Spawn/SoldierSpawn.h"
#include "../SquadLeaderGameModeBase.h"

ASoldierTeam::ASoldierTeam() {
	bReplicates = true;
}


// useless when it's not physicaly in game
void ASoldierTeam::BeginPlay() {
	Super::BeginPlay();
}


void ASoldierTeam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoldierTeam, NbAIBasic);
	DOREPLIFETIME(ASoldierTeam, TeamName);
	DOREPLIFETIME(ASoldierTeam, soldierList);
	DOREPLIFETIME(ASoldierTeam, mainSpawnPoints);

}


void ASoldierTeam::AddSoldierList(ASoldier* newSoldier)
{
	soldierList.AddUnique(newSoldier);
}

void ASoldierTeam::RemoveSoldierList(ASoldier* newSoldier)
{
	if (soldierList.Contains(newSoldier))
		soldierList.Remove(newSoldier);
}

void ASoldierTeam::CleanSoldierList()
{
	soldierList.RemoveAll([](ASoldier* element) {return element == nullptr; });
}


void ASoldierTeam::AddSpawn(ASoldierSpawn* newSpawn)
{
	mainSpawnPoints.AddUnique(newSpawn);
}

void ASoldierTeam::RemoveSpawn(ASoldierSpawn* newSpawn)
{
	if (mainSpawnPoints.Contains(newSpawn))
		mainSpawnPoints.Remove(newSpawn);
}

void ASoldierTeam::CleanSpawnPoints()
{
	mainSpawnPoints.RemoveAll([](ASoldierSpawn* element) {return element == nullptr; });
}

TArray<ASoldierSpawn*> ASoldierTeam::GetUsableSpawnPoints()
{
	TArray<ASoldierSpawn*>result{};
	for (auto element : mainSpawnPoints) {
		if (element->GetUsable())
			result.Add(element);
	}

	return result;
}


void ASoldierTeam::RemoveOneTicket()
{
	Tickets--;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TeamName + TEXT(" : Loses a ticket."));
	
	// TODO : End game here if no tickets left and team is primordial
	if (Tickets == 0 /*&& ...*/) {
		GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, TEXT("END GAME: Tickets depleted\n") +  TeamName + TEXT(" lose !"), false, FVector2D(7,7));
		if (GetLocalRole() == ROLE_Authority) {
			if (ASquadLeaderGameModeBase* GameMode = Cast<ASquadLeaderGameModeBase>(soldierList[0]->GetWorld()->GetAuthGameMode()); GameMode) {  // only for the server
				FTimerHandle timerBeforeClosing;
				soldierList[0]->GetWorld()->GetTimerManager().SetTimer(timerBeforeClosing, GameMode,
					&ASquadLeaderGameModeBase::EndGame, 5.f);  // request to the server to end the game
			}
		}
	}
}