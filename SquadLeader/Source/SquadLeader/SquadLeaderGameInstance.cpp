#include "SquadLeaderGameInstance.h"
#include "SquadLeaderGameModeBase.h"
#include "GameState/SquadLeaderGameState.h"
#include "AbilitySystemGlobals.h"

USquadLeaderGameInstance::USquadLeaderGameInstance() : 
		ListAISquadManagers{}
{
	UAbilitySystemGlobals::Get().InitGlobalData();
}

void USquadLeaderGameInstance::InitAIManagers()
{
	//if(GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Game Instance InitAIManagers()"));
	auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>();

	/*Init AIBasic Manager*/

	FTransform LocationTemp{ {0.f, -1000.f, 0.f}, {0.f,0.f,0.f} };
	AAIBasicManager* AIBasicManager = GetWorld()->SpawnActorDeferred<AAIBasicManager>(AIBasicManagerClass, LocationTemp, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (AIBasicManager) {
		AIBasicManager->FinishSpawning(LocationTemp);
		AIBasicManagerTeam1 = AIBasicManager;
		AIBasicManagerTeam1->Init(GS->GetSoldierTeamCollection()[0]);
	}

	//AIBasicManagerTeam1 = NewObject<AAIBasicManager>(this, AIBasicManagerClass);
	//AIBasicManagerTeam1->Init(gameMode->SoldierTeamCollection[0], GetWorld());

	AAIBasicManager* AIBasicManager2 = GetWorld()->SpawnActorDeferred<AAIBasicManager>(AIBasicManagerClass, LocationTemp, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (AIBasicManager2) {
		AIBasicManager2->FinishSpawning(LocationTemp);
		AIBasicManagerTeam2 = AIBasicManager2;
		AIBasicManagerTeam2->Init(GS->GetSoldierTeamCollection()[1]);
	}


	/*Init AISquad Manager*/
	//Each Player init a SquadManager in SoldierPlayer.cpp PossessedBy
}

void USquadLeaderGameInstance::AddAIBasicToManager(AAIBasicController* AIBasic)
{
	auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>();
	if (!AIBasicManagerTeam1->AIBasicList.Contains(AIBasic) && !AIBasicManagerTeam2->AIBasicList.Contains(AIBasic)) {
		if (GS && Cast<ASoldier>(AIBasic->GetPawn())->GetTeam() == GS->GetSoldierTeamCollection()[0]) {
			AIBasicManagerTeam1->AIBasicList.Add(AIBasic);
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("AIBasic Team 1 added"));
		}
		else if (GS && Cast<ASoldier>(AIBasic->GetPawn())->GetTeam() == GS->GetSoldierTeamCollection()[1]) {
			AIBasicManagerTeam2->AIBasicList.Add(AIBasic);
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("AIBasic Team 2 added"));
		}
		else {
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Une AI n'a pas d'equipe"));
		}
	}
	else {
		//if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Spawned thought the manager"));
	}
	
}

void USquadLeaderGameInstance::InitInfluenceMap() {
	FTransform LocationTemp{ {0.f, 0.f, 0.f}, {0.f,0.f,0.f} };
	AInfluenceMapGrid* _InfluenceMap = GetWorld()->SpawnActorDeferred<AInfluenceMapGrid>(InfluenceMapClass, LocationTemp, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (_InfluenceMap) {
		_InfluenceMap->FinishSpawning(LocationTemp);
		InfluenceMap = _InfluenceMap;
	}
}