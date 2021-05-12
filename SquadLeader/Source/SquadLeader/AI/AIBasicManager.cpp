// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBasicManager.h"
#include "../SquadLeaderGameInstance.h"
#include "../ControlArea/ControlArea.h"
#include "../AI/Mission/CaptureMission.h"
#include "../AI/Mission/PatrolMission.h"
#include "../AI/Mission/PatrolControlAreaMission.h"
#include "../SquadLeaderGameModeBase.h"
#include "../GameState/SquadLeaderGameState.h"

AAIBasicManager::AAIBasicManager() {
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AAIBasicManager::BeginPlay() {
	Super::BeginPlay();
	InitValue();
	InitListSoldierOnControlArea();
	if (nbr_controlArea > 0 && nbr_unite > 0)
		ChooseControlArea();

	//uint8 i = 0;
	//for (auto& Elem : ListSoldierOnControlArea) {
	//	GEngine->AddOnScreenDebugMessage(10 + i, 10.f, FColor::Black, FString::Printf(TEXT("Sur la %i eme zone de controle on a %i IA."), Elem.Key, Elem.Value.SoldierIndex.Num()));
	//	++i;
	//}
}

void AAIBasicManager::InitListSoldierOnControlArea() {
	for (size_t i = 0; i != m_controlAreaManager->GetControlArea().Num(); ++i) {
		ListSoldierOnControlArea.Add(i);
		m_controlAreaManager->GetControlArea()[i]->SetIndexControlArea(i);
		ControlAreaNeutral.Add(m_controlAreaManager->GetControlArea()[i]->GetIndexControlArea());
	}
	//GEngine->AddOnScreenDebugMessage(10, 10.f, FColor::Black, FString::Printf(TEXT("Taille de notre Map : % i"), ListSoldierOnControlArea.Num()));
}

FVector AAIBasicManager::CalculOffSetForInitSpawn(ASoldierSpawn* spawnpoint, int maxNumberBySpawn, int AiNb)
{
	FVector DirSpawn = spawnpoint->GetActorForwardVector();
	DirSpawn = -DirSpawn;  // invert it to clear the front
	FVector LocSpawn = spawnpoint->GetActorLocation();
	float angle = FMath::RadiansToDegrees(FGenericPlatformMath::Acos(FVector::DotProduct({ 1,0,0 }, DirSpawn) / (1 * DirSpawn.Size())));
	if (DirSpawn.Y < 0) angle = -angle;  // regulate the angle signe
	float AnglePerAI = 360.f / maxNumberBySpawn;

	FVector Offset = InitSpawnDiameter;  // take the diameter
	Offset = Offset.RotateAngleAxis(angle, { 0, 0, 1 });
	Offset = Offset.RotateAngleAxis(AnglePerAI * AiNb, { 0, 0, 1 });
	return LocSpawn + Offset;
}

void AAIBasicManager::Init(ASoldierTeam* _Team)
{
	Team = _Team;
	
	// calculate information for placement
	TArray<ASoldierSpawn*> spawnList = Team->GetUsableSpawnPoints();
	int NbAIToSpawn = Team->GetNbAIBasicAssault() + Team->GetNbAIBasicHeavy();
	int maxNumberBySpawn = ceil((NbAIToSpawn+0.0) / spawnList.Num());
		
	for (int spawnLoop = 0; spawnLoop < NbAIToSpawn; spawnLoop++) {
		ASoldierSpawn* spawnpoint = spawnList[spawnLoop % (spawnList.Num())];

		FTransform LocationAI{};
		LocationAI.SetLocation(CalculOffSetForInitSpawn(spawnpoint, maxNumberBySpawn, spawnLoop));
		ASoldierAI* BasicAI;
		if (spawnLoop < Team->GetNbAIBasicAssault()) {
			BasicAI = GetWorld()->SpawnActorDeferred<ASoldierAI>(Team->GetClassBasicAIAssault(), LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		}
		else {
			BasicAI = GetWorld()->SpawnActorDeferred<ASoldierAI>(Team->GetClassBasicAIHeavy(), LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn); 
		}
		if (BasicAI) {
			BasicAI->SpawnDefaultController();
			BasicAI->SetTeam(Team);
			BasicAI->FinishSpawning(LocationAI);
			AAIBasicController* AC = Cast<AAIBasicController>(BasicAI->GetController());
			ensure(AC);
			if (Team->Id == 1)
				AC->SetQueryFilter(m_queryFilter_team1);
			else
				AC->SetQueryFilter(m_queryFilter_team2);

			AC->SetManager(this);
			AC->SetIndexValue(AIBasicList.Num());
			AIBasicList.Add(AC);

			//Add the mission Patroling by default
			UPatrolMission* _patrolMisssion = Cast<UPatrolMission>(NewObject<UPatrolMission>(this, UPatrolMission::StaticClass()));
			_patrolMisssion->InitPatrolMission(1, MissionPriority::eNULL);
			AC->SetMission<UPatrolMission*>(_patrolMisssion);
		}
	}
}

void AAIBasicManager::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(80, 2.f, FColor::Black, TEXT("I'm the AIBasicManager"));*/

	const double StartTime = FPlatformTime::Seconds();

	double TotalTime = 0.0;

	double startTime = FPlatformTime::Seconds();
	
	if ((ControlAreasBeenUpdate || NewSoldierAvailable) && AIBasicAvailable.Num() >= 2) {
		ControlAreasBeenUpdate = false;
		NewSoldierAvailable = false;

		Strategy();
	}

	double endTime = FPlatformTime::Seconds();
	double elapsedTime = endTime - startTime;

	TotalTime += elapsedTime * 1000.0;
	if (AIBasicList.Num() > 0) {
		while (TotalTime <= TimeInterval) {
			if (LastIndex >= AIBasicList.Num())
				LastIndex = 0;
			const double _StartTime = FPlatformTime::Seconds();
			AIBasicList[LastIndex]->HomeTick(DeltaSeconds);
			const double _EndTime = FPlatformTime::Seconds();
			const double _ElapsedTime = _EndTime - _StartTime;

			TotalTime += _ElapsedTime * 1000.0;

			LastIndex++;
		}
	}
	double EndTime = FPlatformTime::Seconds();
	double ElapsedTime = EndTime - StartTime;
	//Petit soucis je pense, car le temps au total est plus petit que le temps de calcul d'une IA....
	//GEngine->AddOnScreenDebugMessage(10, 1.f, FColor::Yellow, FString::Printf(TEXT("Temp total du tick du manager : %f ms"), ElapsedTime * 1000.0));

}

void AAIBasicManager::InitValue() {
	auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>();
	m_controlAreaManager = GS->GetControlAreaManager();
	if (m_controlAreaManager) {
		nbr_controlArea = m_controlAreaManager->GetControlArea().Num();
		nbr_unite = AIBasicList.Num();
	}
}

void AAIBasicManager::ChooseControlArea() {
	int _index_player = 0;
	int _index_control_area = 0;

	int nbr_unit_per_controlArea = nbr_unite / nbr_controlArea;
	while (_index_player < nbr_unite) {
		if (_index_control_area >= nbr_controlArea)
			_index_control_area = 0;
		for (int i = 0; i != nbr_unit_per_controlArea && _index_player < nbr_unite; ++i) {
			UPatrolControlAreaMission* MissionPatrol = Cast<UPatrolControlAreaMission>(NewObject<UPatrolControlAreaMission>(this, UPatrolControlAreaMission::StaticClass()));
			MissionPatrol->InitPatrolControlAreaMission(-1, MissionPriority::eBASIC, m_controlAreaManager->GetControlArea()[_index_control_area]);
			AIBasicList[_index_player]->SetMission<UPatrolControlAreaMission*>(MissionPatrol);

			UCaptureMission* _mission = Cast<UCaptureMission>(NewObject<UCaptureMission>(this, UCaptureMission::StaticClass()));
			_mission->InitCaptureMission(-1, MissionPriority::eMIDDLE, m_controlAreaManager->GetControlArea()[_index_control_area]);
			AIBasicList[_index_player]->SetMission<UCaptureMission*>(_mission);

			ListSoldierOnControlArea.Find(_index_control_area)->SoldierIndex.Add(_index_player);
			_index_player++;
		}
		_index_control_area++;

	}
}

void AAIBasicManager::ChangeAIStatus(const AIAvaibility status, const uint32 IndexSoldier) {
	switch (status) {
	case AIAvaibility::available:
		AIAvailable(IndexSoldier);
		break;
	case AIAvaibility::unavailable:
		AIUnavailable(IndexSoldier);
		break;
	default:
		break;
	}
}

void AAIBasicManager::AIAvailable(const uint32 IndexSoldier) {
	//GEngine->AddOnScreenDebugMessage(10, 1.f, FColor::Purple, TEXT("Une IA est disponible !"));
	if(!AIBasicAvailable.Contains(IndexSoldier))
		AIBasicAvailable.Add(IndexSoldier);
	if(AIBasicUnavailable.Num() > 0 )
		AIBasicUnavailable.Remove(IndexSoldier);

	NewSoldierAvailable = true;
	//GEngine->AddOnScreenDebugMessage(20, 5.f, FColor::Black, FString::Printf(TEXT("Nombre d'IA disponible : %i "), AIBasicAvailable.Num()));
	//GEngine->AddOnScreenDebugMessage(25, 5.f, FColor::Black, FString::Printf(TEXT("Nombre d'IA non disponible : %i "), AIBasicUnavailable.Num()));
}

void AAIBasicManager::AIUnavailable(const uint32 IndexSoldier) {
	//GEngine->AddOnScreenDebugMessage(30, 1.f, FColor::Yellow, TEXT("Une IA n'est plus disponible !"));
	if (AIBasicAvailable.Num() > 0)
		AIBasicAvailable.Remove(IndexSoldier);
	if(!AIBasicUnavailable.Contains(IndexSoldier))
		AIBasicUnavailable.Add(IndexSoldier);
	//GEngine->AddOnScreenDebugMessage(40, 5.f, FColor::Black, FString::Printf(TEXT("Nombre d'IA disponible : %i "), AIBasicAvailable.Num()));
	//GEngine->AddOnScreenDebugMessage(45, 5.f, FColor::Black, FString::Printf(TEXT("Nombre d'IA non disponible : %i "), AIBasicUnavailable.Num()));
}

void AAIBasicManager::UpdateControlArea(const uint8 TeamID, const uint8 IndexControlArea){
	//GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Black, TEXT("Les managers sont au courant qu'un point de controle est prit !"));
	if (TeamID != Team->Id) {
		if (ControlAreaAllies.Num() > 0 && ControlAreaAllies.Contains(IndexControlArea))
			ControlAreaAllies.Remove(IndexControlArea);
		else if (ControlAreaNeutral.Num() > 0 && ControlAreaNeutral.Contains(IndexControlArea))
			ControlAreaNeutral.Remove(IndexControlArea);

		ControlAreaEnnemies.Add(IndexControlArea);
	}
	else {
		if (ControlAreaEnnemies.Num() > 0 && ControlAreaEnnemies.Contains(IndexControlArea))
			ControlAreaEnnemies.Remove(IndexControlArea);
		else if (ControlAreaNeutral.Num() > 0 && ControlAreaNeutral.Contains(IndexControlArea))
			ControlAreaNeutral.Remove(IndexControlArea);
		ControlAreaAllies.Add(IndexControlArea);
	}
	ControlAreasBeenUpdate = true;
	float time = 10.f;
	/*if (Team->Id == 1) {
		GEngine->AddOnScreenDebugMessage(20, time, FColor::Blue, FString::Printf(TEXT("il reste encore %i zone de controle neutre."), ControlAreaNeutral.Num()));
		GEngine->AddOnScreenDebugMessage(30, time, FColor::Blue, FString::Printf(TEXT("il reste encore %i zone de controle alliee."), ControlAreaAllies.Num()));
		GEngine->AddOnScreenDebugMessage(40, time, FColor::Blue, FString::Printf(TEXT("il reste encore %i zone de controle ennemis."), ControlAreaEnnemies.Num()));
	}
	if (Team->Id == 2) {
		GEngine->AddOnScreenDebugMessage(50, time, FColor::Red, FString::Printf(TEXT("il reste encore %i zone de controle neutre."), ControlAreaNeutral.Num()));
		GEngine->AddOnScreenDebugMessage(60, time, FColor::Red, FString::Printf(TEXT("il reste encore %i zone de controle alliee."), ControlAreaAllies.Num()));
		GEngine->AddOnScreenDebugMessage(70, time, FColor::Red, FString::Printf(TEXT("il reste encore %i zone de controle ennemis."), ControlAreaEnnemies.Num()));
	}*/

}

void AAIBasicManager::LostControlArea(const uint8 IndexContolArea) {
	if (AIBasicAvailable.Num() > 0) {
		const double Danger = m_controlAreaManager->GetControlArea()[IndexContolArea]->GetEnnemiInfluenceAverage();
		const float SoldierValue = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode())->InfluenceMap->CharacterInfluenceValue;
		const int Maximum = StaticCast<int>(Danger / SoldierValue) + 2;

		auto Elem = ListSoldierOnControlArea.Find(IndexContolArea);
		uint32 IndexSoldier = 0;
		while (Elem->SoldierIndex.Num() <= Maximum && FindAvailableSoldier(IndexSoldier)) {
			uint32 IndexCA = 0;
			if (AIBasicList[IndexSoldier]->GetIndexControlArea(IndexCA)) {
				UCaptureMission* CaptureMission = Cast<UCaptureMission>(NewObject<UCaptureMission>(this, UCaptureMission::StaticClass()));
				CaptureMission->InitCaptureMission(-1, MissionPriority::eMIDDLE, m_controlAreaManager->GetControlArea()[IndexContolArea]);
				AIBasicList[IndexSoldier]->SetMission<UCaptureMission*>(CaptureMission);

				Elem->SoldierIndex.Add(IndexSoldier);
				ListSoldierOnControlArea.Find(IndexCA)->SoldierIndex.Remove(IndexSoldier);
				AIUnavailable(IndexSoldier);
			}
			if (Team->Id == 1)
				GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Blue, FString::Printf(TEXT("Battre en retraire ! %i IA sont envoy� d�fendre le point !"), Maximum));
			if (Team->Id == 2)
				GEngine->AddOnScreenDebugMessage(20, 5.f, FColor::Red, FString::Printf(TEXT("Battre en retraire ! %i IA sont envoy� d�fendre le point !"), Maximum));
		}
	}
}

void AAIBasicManager::Strategy() {
	//GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Black, FString::Printf(TEXT("Nouvelle Strat�gie")));

	//Voir si on peut envoyer des IAs capturer un point de contr�le neutre
	for (uint32 IndexNeutralControlArea : ControlAreaNeutral) {
		auto Elem = ListSoldierOnControlArea.Find(IndexNeutralControlArea);
		uint32 IndexSoldier = 0;
		while (Elem->SoldierIndex.Num() <= 3 && FindAvailableSoldier(IndexSoldier)) {
			uint32 IndexCA = 0;
			if (AIBasicList[IndexSoldier]->GetIndexControlArea(IndexCA)) {
				UCaptureMission* CaptureMission = Cast<UCaptureMission>(NewObject<UCaptureMission>(this, UCaptureMission::StaticClass()));
				CaptureMission->InitCaptureMission(-1, MissionPriority::eMIDDLE, m_controlAreaManager->GetControlArea()[IndexNeutralControlArea]);
				AIBasicList[IndexSoldier]->SetMission<UCaptureMission*>(CaptureMission);

				Elem->SoldierIndex.Add(IndexSoldier);
				ListSoldierOnControlArea.Find(IndexCA)->SoldierIndex.Remove(IndexSoldier);
				AIUnavailable(IndexSoldier);

				/*const float Timer = 10.f;
				if (Team->Id == 1) {
					GEngine->AddOnScreenDebugMessage(0, Timer, FColor::Purple, FString::Printf(TEXT("L'IA num %i, etait sur la zone de controle num %i"), IndexSoldier, IndexCA));
					GEngine->AddOnScreenDebugMessage(1, Timer, FColor::Purple, FString::Printf(TEXT("L'IA num %i, va sur la zone de controle num %i"), IndexSoldier, IndexNeutralControlArea));
				}
				if (Team->Id == 2) {
					GEngine->AddOnScreenDebugMessage(10, Timer, FColor::Turquoise, FString::Printf(TEXT("L'IA num %i, etait sur la zone de controle num %i"), IndexSoldier, IndexCA));
					GEngine->AddOnScreenDebugMessage(11, Timer, FColor::Turquoise, FString::Printf(TEXT("L'IA num %i, va sur la zone de controle num %i"), IndexSoldier, IndexNeutralControlArea));
				}*/
			}

		}
	}

	//Si possible envoyer les IAs sur des Zone de contr�le Ennemie
	if (AIBasicAvailable.Num() > 0) {
		for (uint32 IndexControlAreaEnnemi : ControlAreaEnnemies) {
			double Danger = m_controlAreaManager->GetControlArea()[IndexControlAreaEnnemi]->GetInfluenceAverage();
			/*if(Team->Id == 2)
				GEngine->AddOnScreenDebugMessage(10 + Team->Id, 5.f, FColor::Red, FString::Printf(TEXT("Danger ennemie : %f."), Danger));
			if (Team->Id == 1)
				GEngine->AddOnScreenDebugMessage(10 + Team->Id, 5.f, FColor::Blue, FString::Printf(TEXT("Danger ennemie : %f."), Danger));
			*/
			const float SoldierValue = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode())->InfluenceMap->CharacterInfluenceValue;
			const int Maximum = StaticCast<int>(Danger / SoldierValue)+ 2;
			//GEngine->AddOnScreenDebugMessage(10 + Team->Id, 5.f, FColor::Blue, FString::Printf(TEXT("Nombre de Soldat suppose envoye  : %i."), Maximum));

			auto Elem = ListSoldierOnControlArea.Find(IndexControlAreaEnnemi);
			uint32 IndexSoldier = 0;
			while (Elem->SoldierIndex.Num() <= Maximum && FindAvailableSoldier(IndexSoldier)) {
				uint32 IndexCA = 0;
				if (AIBasicList[IndexSoldier]->GetIndexControlArea(IndexCA)) {
					UCaptureMission* CaptureMission = Cast<UCaptureMission>(NewObject<UCaptureMission>(this, UCaptureMission::StaticClass()));
					CaptureMission->InitCaptureMission(-1, MissionPriority::eMIDDLE, m_controlAreaManager->GetControlArea()[IndexControlAreaEnnemi]);
					AIBasicList[IndexSoldier]->SetMission<UCaptureMission*>(CaptureMission);

					Elem->SoldierIndex.Add(IndexSoldier);
					ListSoldierOnControlArea.Find(IndexCA)->SoldierIndex.Remove(IndexSoldier);
					AIUnavailable(IndexSoldier);

					/*const float Timer = 10.f;
					if (Team->Id == 1) {
						GEngine->AddOnScreenDebugMessage(0, Timer, FColor::Purple, FString::Printf(TEXT("L'IA num %i, etait sur la zone de controle num %i"), IndexSoldier, IndexCA));
						GEngine->AddOnScreenDebugMessage(1, Timer, FColor::Purple, FString::Printf(TEXT("L'IA num %i, va sur la zone de controle ennemie num %i"), IndexSoldier, IndexControlAreaEnnemi));
					}
					if (Team->Id == 2) {
						GEngine->AddOnScreenDebugMessage(10, Timer, FColor::Turquoise, FString::Printf(TEXT("L'IA num %i, etait sur la zone de controle num %i"), IndexSoldier, IndexCA));
						GEngine->AddOnScreenDebugMessage(11, Timer, FColor::Turquoise, FString::Printf(TEXT("L'IA num %i, va sur la zone de controle ennemie num %i"), IndexSoldier, IndexControlAreaEnnemi));
					}*/
				}
			}
		}
	}

}

bool AAIBasicManager::FindControlAreaOn(const uint8 IndexSoldier, uint32& IndexControlArea) {
	for (auto& Elem : ListSoldierOnControlArea) {
		if (Elem.Value.SoldierIndex.Contains(IndexSoldier))
			IndexControlArea = Elem.Key;
		return true;
	}
	return false;
}

bool AAIBasicManager::FindAvailableSoldier(uint32& IndexSoldier) {
	for (size_t index = 0; index != AIBasicAvailable.Num(); ++index) {
		const uint32 _IndexSoldier = AIBasicAvailable[index];
		uint32 IndexControlArea = 0;
		if (AIBasicList[_IndexSoldier]->GetIndexControlArea(IndexControlArea)) {
			if (ListSoldierOnControlArea.Find(IndexControlArea)->SoldierIndex.Num() > 1) {
				IndexSoldier = _IndexSoldier;
				return true;
			}
		}
	}
	return false;
}