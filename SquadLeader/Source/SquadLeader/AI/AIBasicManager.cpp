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

	if (ControlAreaEnnemies.Num() == 1 && ControlAreaNeutral.Num() == 0) {
		//envoyer toutes les IAs attaquer le point de contrôle et laisser une IA défendre sur les autres point de contrôle
		FinalAttack();
		//GEngine->AddOnScreenDebugMessage(20, 5.f, FColor::Cyan, TEXT("Tout le monde en Attaque !!!!"));
	}
	else if ((ControlAreasBeenUpdate || AIBasicAvailable.Num() > 10)) {
		Strategy();
	}

	double endTime = FPlatformTime::Seconds();
	double elapsedTime = endTime - startTime;

	TotalTime += elapsedTime * 1000.0;
	//GEngine->AddOnScreenDebugMessage(Team->Id, 1.f, FColor::Red, FString::Printf(TEXT("On a %i IA !"), AIBasicList.Num()));

	if (AIBasicList.Num() > 0) {
		size_t NumberTickAI = 0;
		while (TotalTime <= TimeInterval && NumberTickAI < AIBasicList.Num()) {
			if (LastIndex >= AIBasicList.Num())
				LastIndex = 0;
			const double _StartTime = FPlatformTime::Seconds();
			AIBasicList[LastIndex]->HomeTick(DeltaSeconds);
			const double _EndTime = FPlatformTime::Seconds();
			const double _ElapsedTime = _EndTime - _StartTime;

			TotalTime += _ElapsedTime * 1000.0;

			++LastIndex;
			++NumberTickAI;
		}
		//GEngine->AddOnScreenDebugMessage(10 + Team->Id, 1.f, FColor::Blue, FString::Printf(TEXT("On a fait ticker %i IA !"), NumberTickAI));
	}
	double EndTime = FPlatformTime::Seconds();
	double ElapsedTime = EndTime - StartTime;
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

	const uint8 NbrControlArea = (m_controlAreaManager->GetControlArea().Num() / 2) + 1;
	//GEngine->AddOnScreenDebugMessage(10 + Team->Id, 5.f, FColor::Blue, FString::Printf(TEXT("Nombre de control area au départ = %i"), NbrControlArea));
	const uint16 NbrUnite = AIBasicList.Num() / NbrControlArea;
	//GEngine->AddOnScreenDebugMessage(10 + Team->Id, 5.f, FColor::Blue, FString::Printf(TEXT("Nombre unite au départ = %i"), NbrUnite));

	TArray<AControlArea*> ControlAreas;
	ControlAreas.Init(nullptr, NbrControlArea);
	//GEngine->AddOnScreenDebugMessage(10 + Team->Id, 5.f, FColor::Blue, FString::Printf(TEXT("Taille de la liste : %i"), ControlAreas.Num()));
	FVector LocalPosition;
	if (Team->GetUsableSpawnPoints().Num() > 0)
		LocalPosition = Team->GetUsableSpawnPoints()[0]->GetActorLocation();
	else
		LocalPosition = FVector();

	auto Minimum = [&](const AControlArea* ControlArea1, const AControlArea* ControlArea2) {
		const float distance1 = FVector::Dist2D(LocalPosition, ControlArea1->GetActorLocation());
		const float distance2 = FVector::Dist2D(LocalPosition, ControlArea2->GetActorLocation());

		return distance1 < distance2;
	};

	TArray<AControlArea*> AllControlAreaBis = m_controlAreaManager->GetControlArea();
	size_t IndexControlArea = 0;

	while (IndexControlArea != NbrControlArea) {
		AControlArea* ControlArea = AllControlAreaBis[0];
		size_t IndexCotrolAreaMinim = 0;
		for (size_t index = 1; index != AllControlAreaBis.Num(); ++index) {
			if (!Minimum(ControlArea, AllControlAreaBis[index])) {
				ControlArea = AllControlAreaBis[index];
				IndexCotrolAreaMinim = index;
			}
		}
		ControlAreas[IndexControlArea] = ControlArea;
		++IndexControlArea;
		AllControlAreaBis.RemoveAt(IndexCotrolAreaMinim);
	}

	IndexControlArea = 0;

	for (size_t Indexsoldier = 0; Indexsoldier != AIBasicList.Num(); ++Indexsoldier) {
		if (Indexsoldier == NbrUnite * (IndexControlArea + 1))
			++IndexControlArea;

		if (IndexControlArea >= ControlAreas.Num())
			--IndexControlArea;

		AIBasicList[Indexsoldier]->EmptyMissionList();

		UPatrolControlAreaMission* MissionPatrol = Cast<UPatrolControlAreaMission>(NewObject<UPatrolControlAreaMission>(this, UPatrolControlAreaMission::StaticClass()));
		MissionPatrol->InitPatrolControlAreaMission(-1, MissionPriority::eBASIC, ControlAreas[IndexControlArea]);
		AIBasicList[Indexsoldier]->SetMission<UPatrolControlAreaMission*>(MissionPatrol);

		UCaptureMission* _mission = Cast<UCaptureMission>(NewObject<UCaptureMission>(this, UCaptureMission::StaticClass()));
		_mission->InitCaptureMission(-1, MissionPriority::eMIDDLE, ControlAreas[IndexControlArea]);
		AIBasicList[Indexsoldier]->SetMission<UCaptureMission*>(_mission);

		const uint32 ControlAreaIndex = ControlAreas[IndexControlArea]->GetIndexControlArea();
		ListSoldierOnControlArea.Find(ControlAreaIndex)->SoldierIndex.Add(Indexsoldier);

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
		const int Maximum = StaticCast<int>(Danger / 0.1f) * 2;

		auto Elem = ListSoldierOnControlArea.Find(IndexContolArea);
		uint32 IndexSoldier = 0;
		while (Elem->SoldierIndex.Num() <= Maximum && FindAvailableSoldier(IndexSoldier, IndexContolArea)) {
			uint32 IndexCA = 0;
			if (AIBasicList[IndexSoldier]->GetIndexControlArea(IndexCA)) {
				//GEngine->AddOnScreenDebugMessage(50, 1.f, FColor::Blue, TEXT("Part defendre une zone de controle"));
				UCaptureMission* CaptureMission = Cast<UCaptureMission>(NewObject<UCaptureMission>(this, UCaptureMission::StaticClass()));
				CaptureMission->InitCaptureMission(-1, MissionPriority::eMIDDLE, m_controlAreaManager->GetControlArea()[IndexContolArea]);
				AIBasicList[IndexSoldier]->SetMission<UCaptureMission*>(CaptureMission);

				Elem->SoldierIndex.Add(IndexSoldier);
				ListSoldierOnControlArea.Find(IndexCA)->SoldierIndex.Remove(IndexSoldier);
				AIUnavailable(IndexSoldier);
			}
			/*if (Team->Id == 1)
				GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Blue, FString::Printf(TEXT("Battre en retraire ! %i IA sont envoyé défendre le point !"), Maximum));
			if (Team->Id == 2)
				GEngine->AddOnScreenDebugMessage(20, 5.f, FColor::Red, FString::Printf(TEXT("Battre en retraire ! %i IA sont envoyé défendre le point !"), Maximum));*/
		}
	}
}

void AAIBasicManager::Strategy() {
	//GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Black, FString::Printf(TEXT("Nouvelle Stratégie")));

	//Si possible envoyer les IAs sur des Zone de contrôle Ennemie
	if (AIBasicAvailable.Num() > 0) {
		for (uint32 IndexControlAreaEnnemi : ControlAreaEnnemies) {
			double Danger = m_controlAreaManager->GetControlArea()[IndexControlAreaEnnemi]->GetInfluenceAverage();
			/*if(Team->Id == 2)
				GEngine->AddOnScreenDebugMessage(10 + Team->Id, 5.f, FColor::Red, FString::Printf(TEXT("Danger ennemie : %f."), Danger));
			if (Team->Id == 1)
				GEngine->AddOnScreenDebugMessage(10 + Team->Id, 5.f, FColor::Blue, FString::Printf(TEXT("Danger ennemie : %f."), Danger));
			*/
			const float SoldierValue = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode())->InfluenceMap->CharacterInfluenceValue;
			const int Maximum = StaticCast<int>(Danger / 0.1f) * 2;
			//GEngine->AddOnScreenDebugMessage(10 + Team->Id, 5.f, FColor::Blue, FString::Printf(TEXT("Nombre de Soldat suppose envoye  : %i."), Maximum));
			
			if (Maximum <= AIBasicAvailable.Num()) {
				auto Elem = ListSoldierOnControlArea.Find(IndexControlAreaEnnemi);
				uint32 IndexSoldier = 0;
				while (Elem->SoldierIndex.Num() <= Maximum && FindAvailableSoldier(IndexSoldier, IndexControlAreaEnnemi)) {
					uint32 IndexCA = 0;
					if (AIBasicList[IndexSoldier]->GetIndexControlArea(IndexCA)) {

						AIBasicList[IndexSoldier]->EmptyMissionList();

						UPatrolControlAreaMission* MissionPatrol = Cast<UPatrolControlAreaMission>(NewObject<UPatrolControlAreaMission>(this, UPatrolControlAreaMission::StaticClass()));
						MissionPatrol->InitPatrolControlAreaMission(-1, MissionPriority::eBASIC, m_controlAreaManager->GetControlArea()[IndexControlAreaEnnemi]);
						AIBasicList[IndexSoldier]->SetMission<UPatrolControlAreaMission*>(MissionPatrol);

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
					ControlAreasBeenUpdate = false;
					NewSoldierAvailable = false;
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

bool AAIBasicManager::FindAvailableSoldier(uint32& IndexSoldier, const uint32 IndexControlArea) {

	if (AIBasicAvailable.Num() > 0) {
		const FVector ControlAreaLocation = m_controlAreaManager->GetControlArea()[IndexControlArea]->GetActorLocation();
		IndexSoldier = AIBasicAvailable[0];
		float DistanceMinimal = FVector::Dist2D(ControlAreaLocation, AIBasicList[IndexSoldier]->GetPawn()->GetActorLocation());


		bool result = false;
		uint32 _IndexControlArea = 0;

		if (AIBasicList[IndexSoldier]->GetIndexControlArea(_IndexControlArea))
			if (ListSoldierOnControlArea.Find(_IndexControlArea)->SoldierIndex.Num() > 2)
				result = true;

		for (size_t index = 1; index != AIBasicAvailable.Num(); ++index) {
			const uint32 IndexSoldierAvailable = AIBasicAvailable[index];
			uint32 IndexControlArea = 0;
			if (AIBasicList[IndexSoldierAvailable]->GetIndexControlArea(IndexControlArea)) {
				const float dist = FVector::Dist2D(ControlAreaLocation, AIBasicList[IndexSoldierAvailable]->GetPawn()->GetActorLocation());
				if (ListSoldierOnControlArea.Find(IndexControlArea)->SoldierIndex.Num() > 2 && dist <= DistanceMinimal) {
					IndexSoldier = IndexSoldierAvailable;
					DistanceMinimal = dist;
					result = true;
				}
			}
		}
		return result;
	}
	return false;
}

void AAIBasicManager::FinalAttack() {
	const uint32 IndexControlAreaEnnemi = ControlAreaEnnemies[0];
	auto LastEnnemieControlArea = ListSoldierOnControlArea.Find(IndexControlAreaEnnemi);

	if (m_controlAreaManager->GetControlArea()[IndexControlAreaEnnemi] != nullptr) {
		for (size_t IndexSoldier = 0; IndexSoldier != AIBasicAvailable.Num(); ++IndexSoldier) {
			uint32 IndexCA = 0;
			if (AIBasicList[IndexSoldier]->GetIndexControlArea(IndexCA)) {
				UCaptureMission* CaptureMission = Cast<UCaptureMission>(NewObject<UCaptureMission>(this, UCaptureMission::StaticClass()));
				CaptureMission->InitCaptureMission(-1, MissionPriority::eMIDDLE, m_controlAreaManager->GetControlArea()[IndexControlAreaEnnemi]);
				AIBasicList[IndexSoldier]->SetMission<UCaptureMission*>(CaptureMission);

				LastEnnemieControlArea->SoldierIndex.Add(IndexSoldier);
				ListSoldierOnControlArea.Find(IndexCA)->SoldierIndex.Remove(IndexSoldier);
			}
		}
	}
}

void AAIBasicManager::FinalDefens() {
}

float AAIBasicManager::DistanceSoldierControlArea(const AAIBasicController* Soldier, const AControlArea* ControlArea) {
	const FVector SoldierPosition = Soldier->GetPawn()->GetActorLocation();
	const FVector ControlAreaPosition = ControlArea->GetActorLocation();

	return FVector::Dist2D(SoldierPosition, ControlAreaPosition);
}