// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaManager.h"
#include "../SquadLeaderGameModeBase.h"


AControlAreaManager::AControlAreaManager()
{
	bReplicates = true;
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


TArray<AControlArea*> AControlAreaManager::GetAreaControlledByTeam(TSubclassOf<ASoldierTeam> _Team)
{
	TArray<AControlArea*> selection;
	for (auto element : ControlAreaList) {
		if (element->isTakenBy == _Team) {
			selection.Add(element);
		}
	}
	return selection;
}


TSubclassOf<ASoldierTeam> AControlAreaManager::GetTeamWithMostControl()  // to try with UI
{
	if (auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); gameMode) {  // only for the server
		int nbControleAreaToObtain = ControlAreaList.Num() - (ControlAreaList.Num() / 2); /*TODO: use teams politic here*/
		for (TSubclassOf<ASoldierTeam> team : gameMode->SoldierTeamCollection) {
			if (GetAreaControlledByTeam(team).Num() >= nbControleAreaToObtain) {
				return team;
			}
		}
	}
	return TSubclassOf<ASoldierTeam>();
}

TSubclassOf<ASoldierTeam> AControlAreaManager::GetTeamWithAllControl()
{
	if (ControlAreaList.Num() > 0) {
		for (auto element : ControlAreaList) {
			if (element->isTakenBy != ControlAreaList[0]->isTakenBy) {
				return TSubclassOf<ASoldierTeam>();
			}
		}
		return ControlAreaList[0]->isTakenBy;
	}
	return TSubclassOf<ASoldierTeam>();
}
