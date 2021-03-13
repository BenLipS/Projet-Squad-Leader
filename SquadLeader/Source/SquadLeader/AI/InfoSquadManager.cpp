// Fill out your copyright notice in the Description page of Project Settings.


#include "InfoSquadManager.h"

#include "../Soldiers/AIs/SoldierAI.h"
#include "AISquadController.h"

AInfoSquadManager::AInfoSquadManager()
{
	bReplicates = true;
}

void AInfoSquadManager::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("INFOMANAGER SUCCESSFULLY CREATED ON CLIENT"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("INFOMANAGER SUCCESSFULLY CREATED ON SERVER"));
	}
}

void AInfoSquadManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInfoSquadManager, SquadAI);
}

void AInfoSquadManager::OnSquadManagerChange(TArray<class AAISquadController*> controllerSquad)
{
	SquadAI.Empty();
	for (AAISquadController* controller : controllerSquad)
	{
		ASoldierAI* temp = controller->GetPawn<ASoldierAI>();
		if (temp)
		{
			SquadAI.Add(temp);
		}
	}
}

void AInfoSquadManager::OnRepSquadAIChanged()
{
	OnSquadAIChanged.Broadcast(SquadAI);
}