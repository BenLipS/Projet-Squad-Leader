// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBasicManager.h"
#include "../SquadLeaderGameInstance.h"
#include "../ControlArea/ControlArea.h"
#include "../AI/Mission/CaptureMission.h"

AAIBasicManager::AAIBasicManager() {
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AAIBasicManager::BeginPlay() {
	Super::BeginPlay();
	InitValue();
	if (nbr_controlArea > 0)
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

void AAIBasicManager::Init(TSubclassOf<ASoldierTeam> _Team)
{
	Team = _Team;
	
	// calculate information for placement
	TArray<ASoldierSpawn*> spawnList = Team.GetDefaultObject()->GetUsableSpawnPoints();
	int NbAIToSpawn = Team.GetDefaultObject()->NbAIBasic;
	int maxNumberBySpawn = ceil((NbAIToSpawn+0.0) / spawnList.Num());
		
	for (int spawnLoop = 0; spawnLoop < NbAIToSpawn; spawnLoop++) {
		ASoldierSpawn* spawnpoint = spawnList[spawnLoop % (spawnList.Num())];

		FTransform LocationAI{};
		LocationAI.SetLocation(CalculOffSetForInitSpawn(spawnpoint, maxNumberBySpawn, spawnLoop));
		ASoldierAI* BasicAI = GetWorld()->SpawnActorDeferred<ASoldierAI>(Team.GetDefaultObject()->GetClassBasicAI(), LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
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
	ASquadLeaderGameModeBase* _Gamemode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
	m_controlAreaManager = Cast<AControlAreaManager>(_Gamemode->ControlAreaManager->GetDefaultObject());
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
			UMission* _mission_m = NewObject<UMission>(this,UMission::StaticClass());
			UCaptureMission* _mission = Cast<UCaptureMission>(_mission_m);
			_mission->InitCaptureMission(-1, MissionPriority::eBASIC, m_controlAreaManager->GetControlArea()[_index_control_area]);
			AIBasicList[_index_player]->SetMission(_mission);
			_index_player++;
		}
		_index_control_area++;
	}
}