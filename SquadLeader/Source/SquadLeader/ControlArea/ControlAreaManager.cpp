// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaManager.h"


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
	return TArray<AControlArea*>();
}


TSubclassOf<ASoldierTeam> AControlAreaManager::GetTeamWithMostControl()
{
	return TSubclassOf<ASoldierTeam>();
}