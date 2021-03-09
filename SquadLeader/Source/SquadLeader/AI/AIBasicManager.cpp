// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBasicManager.h"
#include "../SquadLeaderGameInstance.h"
#include "../ControlArea/ControlArea.h"
#include "../SquadLeaderGameModeBase.h"
#include "../AI/Mission.h"

AAIBasicManager::AAIBasicManager() {
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AAIBasicManager::BeginPlay() {
	Super::BeginPlay();
	InitValue();
	if(nbr_controlArea > 0)
		ChooseCOntrolArea();
}

void AAIBasicManager::Init(TSubclassOf<ASoldierTeam> _Team)
{
	Team = _Team;

	/*For now Each AIBasicManager Spawn with 4 AIs*/
	/*TEMPORARY*/
	//if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, TEXT("Init AIBasicManger"));
	auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
	FVector Start1{};
	FVector Start2{};
	FVector Start3{};

	if (Team == gameMode->SoldierTeamCollection[0]) {
		Start1 = { 19000.f, 10000.f, 125.f }; //Start = { 5490.f, 1200.f, 87.999985f }; //Start = { 8760.f, 15590.f, 87.999985f };
		Start2 = { 19000.f, 19000.f, 430.f };
		Start3 = { 19000.f, 1000.f, 430.f };
	}
	else if (Team == gameMode->SoldierTeamCollection[1]){
		Start1 = { 1000.f, 10000.f, 125.f }; //Start = { 2830.f, 4590.f, 87.999985f }; //Start = { 16350.f, 15590.f, 87.999985f };
		Start2 = { 1900.f, 19000.f, 430.f };
		Start3 = { 1000.f, 1000.f, 430.f };
	}
	
	FVector OffSet{ 0.f, 500.f, 0.f };

	FTransform LocationAI{  };
	LocationAI.SetLocation(Start1);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // La maniere de faire le respawn
	ASoldierAI* BasicAI = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (BasicAI) {
		BasicAI->SpawnDefaultController();
		BasicAI->SetTeam(Team);
		BasicAI->FinishSpawning(LocationAI);
		AIBasicList.Add(Cast<AAIBasicController>(BasicAI->GetController()));
	}

	FTransform LocationAI1{};
	LocationAI1.SetLocation(LocationAI.GetLocation() + OffSet);
	ASoldierAI* BasicAI1 = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (BasicAI1) {
		BasicAI1->SpawnDefaultController();
		BasicAI1->SetTeam(Team);
		BasicAI1->FinishSpawning(LocationAI1);
		AIBasicList.Add(Cast<AAIBasicController>(BasicAI1->GetController()));
	}

	FTransform LocationAI2{};
	LocationAI2.SetLocation(Start2);
	ASoldierAI* BasicAI2 = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (BasicAI2) {
		BasicAI2->SpawnDefaultController();
		BasicAI2->SetTeam(Team);
		BasicAI2->FinishSpawning(LocationAI2);
		AIBasicList.Add(Cast<AAIBasicController>(BasicAI2->GetController()));
	}

	FTransform LocationAI3{};
	LocationAI3.SetLocation(LocationAI2.GetLocation() + OffSet);
	ASoldierAI* BasicAI3 = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (BasicAI3) {
		BasicAI3->SpawnDefaultController();
		BasicAI3->SetTeam(Team);
		BasicAI3->FinishSpawning(LocationAI3);
		AIBasicList.Add(Cast<AAIBasicController>(BasicAI3->GetController()));
	}

	FTransform LocationAI4{};
	LocationAI4.SetLocation(Start3);
	ASoldierAI* BasicAI4 = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (BasicAI4) {
		BasicAI4->SpawnDefaultController();
		BasicAI4->SetTeam(Team);
		BasicAI4->FinishSpawning(LocationAI4);
		AIBasicList.Add(Cast<AAIBasicController>(BasicAI4->GetController()));
	}

	FTransform LocationAI5{};
	LocationAI5.SetLocation(LocationAI4.GetLocation() + OffSet);
	ASoldierAI* BasicAI5 = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (BasicAI5) {
		BasicAI5->SpawnDefaultController();
		BasicAI5->SetTeam(Team);
		BasicAI5->FinishSpawning(LocationAI5);
		AIBasicList.Add(Cast<AAIBasicController>(BasicAI5->GetController()));
	}
	
}

void AAIBasicManager::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(80, 2.f, FColor::Black, TEXT("I'm the AIBasicManager"));*/
	
}

void AAIBasicManager::InitValue() {
	ASquadLeaderGameModeBase* _Gamemode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
	m_controlAreaManager = Cast<AControlAreaManager>(_Gamemode->ControlAreaManager->GetDefaultObject());
	if (m_controlAreaManager) {
		nbr_controlArea = m_controlAreaManager->GetControlArea().Num();
		nbr_unite = AIBasicList.Num();
	}
}

void AAIBasicManager::ChooseCOntrolArea() {
	int _index_player = 0;
	int _index_control_area = 0;
	int nbr_unit_per_controlArea = nbr_unite / nbr_controlArea;

	while (_index_player < nbr_unite) {
		if (_index_control_area >= nbr_controlArea)
			_index_control_area = 0;
		for (int i = 0; i != nbr_unit_per_controlArea; ++i) {
			UMission* _mission = NewObject<UMission>(this, UMission::StaticClass());;
			_mission->Type = MissionType::MoveTo;
			_mission->Location = m_controlAreaManager->GetControlArea()[_index_control_area]->GetActorLocation();
			AIBasicList[_index_player]->SetMission(_mission);
			_index_player++;
		}
		_index_control_area++;
	}
}