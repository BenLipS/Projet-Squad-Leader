// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaManager.h"
#include "../GameState/SquadLeaderGameState.h"
#include "../SquadLeaderGameModeBase.h"


AControlAreaManager::AControlAreaManager()
{
	bReplicates = true;
}


void AControlAreaManager::PreInitialisation()
{
	if (auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS) {
		GS->SetControlAreaManager(this);
	}
}

int AControlAreaManager::getpriority()
{
	return 1;
}

void AControlAreaManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AControlAreaManager, ControlAreaList, COND_None, REPNOTIFY_Always);
}


void AControlAreaManager::AddControlArea(AControlArea* _NewArea)
{
	ControlAreaList.AddUnique(_NewArea);
}

void AControlAreaManager::CleanControlAreaList()
{
	ControlAreaList.RemoveAll([](AControlArea* element) {return element == nullptr; });
}


TArray<AControlArea*> AControlAreaManager::GetAreaControlledByTeam(ASoldierTeam* _Team)
{
	TArray<AControlArea*> selection;
	for (auto element : ControlAreaList) {
		if (element->isTakenBy == _Team) {
			selection.Add(element);
		}
	}
	return selection;
}


ASoldierTeam* AControlAreaManager::GetTeamWithMostControl()  // to try with UI
{
	if (GetLocalRole() == ROLE_Authority) {  // only for the server
		if (auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS) {
			int nbControleAreaToObtain = ControlAreaList.Num() - (ControlAreaList.Num() / 2); /*TODO: use teams politic here*/
			for (ASoldierTeam* team : GS->GetSoldierTeamCollection()) {
				if (GetAreaControlledByTeam(team).Num() >= nbControleAreaToObtain) {
					return team;
				}
			}
		}
	}
	return nullptr;
}

ASoldierTeam* AControlAreaManager::GetTeamWithAllControl()
{
	if (ControlAreaList.Num() > 0) {
		for (auto element : ControlAreaList) {
			if (element->isTakenBy != ControlAreaList[0]->isTakenBy) {
				return nullptr;
			}
		}
		return ControlAreaList[0]->isTakenBy;
	}
	return nullptr;
}
