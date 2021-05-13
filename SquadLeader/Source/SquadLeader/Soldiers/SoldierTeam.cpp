#include "SoldierTeam.h"
#include "Soldier.h"
#include "Players/SoldierPlayerController.h"
#include "../UI/Interface/TicketInterface.h"
#include "../Spawn/SoldierSpawn.h"
#include "../GameState/SquadLeaderGameState.h"
#include "../SquadLeaderGameModeBase.h"
#include "SquadLeader/Soldiers/AIs/SoldierAI.h"

void ASoldierTeam::BroadcastTickets()
{
	OnTicketChanged.Broadcast(Tickets);
}

ASoldierTeam::ASoldierTeam() {
	bReplicates = true;
}


void ASoldierTeam::PreInitialisation()
{
	if (auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS) {
		GS->AddSoldierTeam(this);
	}

	// fetch data from the gamemode (so only on server)
	if (auto GM = GetWorld()->GetAuthGameMode<ASquadLeaderGameModeBase>(); GM) {
		// ticket number
		if (OverrideTickets < 0) {
			// take gameMode data
			Tickets = GM->GetBaseTicketForTeam();
		}
		else {
			// take override data
			Tickets = OverrideTickets;
		}

		// number of basic assault AI
		if (OverrideNbAIBasicAssault < 0) {
			// take gameMode data
			NbAIBasicAssault = GM->GetBaseAIBasicAssaultNumberForTeam();
		}
		else {
			// take override data
			NbAIBasicAssault = OverrideNbAIBasicAssault;
		}
		
		// number of basic heavy AI
		if (OverrideNbAIBasicHeavy < 0) {
			// take gameMode data
			NbAIBasicHeavy = GM->GetBaseAIBasicHeavyNumberForTeam();
		}
		else {
			// take override data
			NbAIBasicHeavy = OverrideNbAIBasicHeavy;
		}
	}
}

int ASoldierTeam::GetPriority() const
{
	return 1;
}

// useless when it's not physicaly in game
void ASoldierTeam::BeginPlay() {
	Super::BeginPlay();
}


void ASoldierTeam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoldierTeam, NbAIBasicAssault);
	DOREPLIFETIME(ASoldierTeam, NbAIBasicHeavy);
	DOREPLIFETIME(ASoldierTeam, TeamName);
	DOREPLIFETIME(ASoldierTeam, SoldierList);
	DOREPLIFETIME(ASoldierTeam, mainSpawnPoints);
	DOREPLIFETIME(ASoldierTeam, Tickets);
}

TSubclassOf<ASoldierAI> ASoldierTeam::GetClassBasicAI()
{
	return ClassBasicAI;
}

TSubclassOf<class ASoldierAI> ASoldierTeam::GetClassBasicAIAssault()
{
	return ClassBasicAIAssault;
}

TSubclassOf<class ASoldierAI> ASoldierTeam::GetClassBasicAIHeavy()
{
	return ClassBasicAIHeavy;
}

void ASoldierTeam::AddSoldierList(ASoldier* NewSoldier)
{
	SoldierList.AddUnique(NewSoldier);
	OnSoldierAddedToList.Broadcast(NewSoldier);
}

void ASoldierTeam::RemoveSoldierList(ASoldier* NewSoldier)
{
	SoldierList.Remove(NewSoldier);
	OnSoldierRemovedFromList.Broadcast(NewSoldier);
}

void ASoldierTeam::CleanSoldierList()
{
	SoldierList.RemoveAll([](ASoldier* element) {return element == nullptr; });
}

void ASoldierTeam::AddSpawn(ASoldierSpawn* newSpawn)
{
	if (GetLocalRole() == ROLE_Authority)
		mainSpawnPoints.AddUnique(newSpawn);
	CleanSpawnPoints();
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

TArray<ASoldier*> ASoldierTeam::GetSoldierList() const
{
	return SoldierList;
}

void ASoldierTeam::RemoveOneTicket()
{
	Tickets--;
}

void ASoldierTeam::RemoveTickets(const float _NbTickets)
{
	Tickets -= _NbTickets;
}