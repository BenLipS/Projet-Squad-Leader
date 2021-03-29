// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBasicManager.h"
#include "../SquadLeaderGameInstance.h"
#include "../ControlArea/ControlArea.h"
#include "../GameState/SquadLeaderGameState.h"
#include "../AI/Mission.h"

AAIBasicManager::AAIBasicManager() {
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AAIBasicManager::BeginPlay() {
	Super::BeginPlay();
	InitValue();
	if(nbr_controlArea > 0)
		ChooseControlArea();
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
	int NbAIToSpawn = Team->NbAIBasic;
	int maxNumberBySpawn = ceil((NbAIToSpawn+0.0) / spawnList.Num());
		
	for (int spawnLoop = 0; spawnLoop < NbAIToSpawn; spawnLoop++) {
		ASoldierSpawn* spawnpoint = spawnList[spawnLoop % (spawnList.Num())];

		FTransform LocationAI{};
		LocationAI.SetLocation(CalculOffSetForInitSpawn(spawnpoint, maxNumberBySpawn, spawnLoop));
		ASoldierAI* BasicAI = GetWorld()->SpawnActorDeferred<ASoldierAI>(Team->GetClassBasicAI(), LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (BasicAI) {
			BasicAI->SpawnDefaultController();
			BasicAI->SetTeam(Team);
			BasicAI->FinishSpawning(LocationAI);

			AAIBasicController* AC = Cast<AAIBasicController>(BasicAI->GetController());
			ensure(AC);
			AIBasicList.Add(AC);
		}
	}
}

void AAIBasicManager::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(80, 2.f, FColor::Black, TEXT("I'm the AIBasicManager"));*/
	
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

	while (_index_player < nbr_unite) {
		UMission* _mission = NewObject<UMission>(this, UMission::StaticClass());;
		_mission->Type = MissionType::MoveTo;
		_mission->Location = m_controlAreaManager->GetControlArea()[_index_player % nbr_controlArea]->GetActorLocation();
		AIBasicList[_index_player]->SetMission(_mission);

		_index_player++;
	}
}