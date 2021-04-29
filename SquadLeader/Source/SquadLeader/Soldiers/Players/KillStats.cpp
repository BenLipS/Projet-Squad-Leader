// Fill out your copyright notice in the Description page of Project Settings.


#include "KillStats.h"

AKillStats::AKillStats() {
	bReplicates = true;
}

void AKillStats::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AKillStats, NbDeathByAI, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AKillStats, NbDeathByPlayer, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AKillStats, NbKillAI, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AKillStats, NbKillPlayer, COND_None, REPNOTIFY_Always);
}