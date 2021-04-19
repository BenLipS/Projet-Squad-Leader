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

void ASoldierTeam::InformAllPlayerController_Implementation()
{
	for (auto PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; PCIterator++)
	{
		if (auto PC = Cast<ASoldierPlayerController>(PCIterator->Get()); PC)
		{
			if (PC->GetTeam() == this)
			{
				PC->OnAllyTicket_Received(Tickets);
			}
			else
			{
				PC->OnEnnemyTicket_Received(Tickets);
			}
		}
	}
}

void ASoldierTeam::RemoveOneTicket()
{
	Tickets--;

	InformAllPlayerController();

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TeamName + TEXT(" : Loses a ticket."));

	// TODO : End game here if no tickets left and team is primordial
	if (ASquadLeaderGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASquadLeaderGameModeBase>(); GameMode) {  // only for the server
		GameMode->CheckTeamTicketsVictoryCondition();
	}
}