// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquadManager.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include<algorithm>
// temp include, need to be replace by more robust code
#include "../Soldiers/Soldier.h"

AAISquadManager::AAISquadManager() {
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}

void AAISquadManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// everyone
	DOREPLIFETIME(AAISquadManager, AISquadList);
}

void AAISquadManager::BeginPlay()
{
	Super::BeginPlay();
	OnSquadChanged.Broadcast(AISquadList);
}

void AAISquadManager::OnRep_AISquadList()
{
	OnSquadChanged.Broadcast(AISquadList);
}

void AAISquadManager::Init(TSubclassOf<ASoldierTeam> _Team, ASoldierPlayer* _Player)
{
	Team = _Team;
	Leader = _Player;
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Init Squad Manager for: %s"), *Leader->GetName()));

	FTransform PlayerTransform = Leader->GetTransform();
	FVector Offset_{ 300.f,0.f,0.f };
	FVector Offset_1{ 0.f,300.f,0.f };
	FTransform LocationAI = PlayerTransform;
	FTransform LocationAI1 = PlayerTransform;
	FTransform LocationAI2 = PlayerTransform;
	LocationAI.SetLocation(PlayerTransform.GetLocation() + Offset_1);
	LocationAI1.SetLocation(PlayerTransform.GetLocation() - Offset_1);
	LocationAI2.SetLocation(PlayerTransform.GetLocation() + Offset_);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // La maniere de faire le respawn
	ASoldierAI* SquadAI = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn); 
	if (SquadAI) {
		SquadAI->SpawnDefaultController();
		SquadAI->FinishSpawning(LocationAI);
		AISquadList.Add(Cast<AAISquadController>(SquadAI->Controller));
		Cast<AAISquadController>(SquadAI->Controller)->SquadManager = this;
	}
	ASoldierAI* SquadAI1 = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI1, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (SquadAI1) {
		SquadAI1->SpawnDefaultController();
		SquadAI1->FinishSpawning(LocationAI1);
		AISquadList.Add(Cast<AAISquadController>(SquadAI1->Controller));
		Cast<AAISquadController>(SquadAI1->Controller)->SquadManager = this;
	}
	ASoldierAI* SquadAI2 = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI2, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (SquadAI2) {
		SquadAI2->SpawnDefaultController();
		SquadAI2->FinishSpawning(LocationAI2);
		AISquadList.Add(Cast<AAISquadController>(SquadAI2->Controller));
		Cast<AAISquadController>(SquadAI2->Controller)->SquadManager = this;
	}

	Mission = NewObject<UMission>(this, UMission::StaticClass());
	Mission->Type = MissionType::Formation;
	TypeOfFormation = FormationType::Circle;
}

void AAISquadManager::Tick(float DeltaTime)
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Tick for: %s"), *Leader->GetName()));

	if (Mission->Type == MissionType::Formation) {
		UpdateFormation();
	}

	Super::Tick(DeltaTime);
		
}

void AAISquadManager::UpdateFormation()
{
	switch (TypeOfFormation) {
	case FormationType::Circle:
		UpdateCircleFormation();
		break;
	case FormationType::Arrow:
		break;
	default: 
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Wrong Formation Type"));
	}

	for (int i = 0; i < AISquadList.Num(); i++) {
		AISquadList[i]->get_blackboard()->SetValueAsVector("FormationLocation", FormationPos[i]);
	}
}

void AAISquadManager::UpdateCircleFormation()
{
	FVector DirPlayer = Leader->GetActorForwardVector();
	DirPlayer = -DirPlayer;
	FVector LocPlayer = Leader->GetActorLocation();
	float angle = FMath::RadiansToDegrees(FGenericPlatformMath::Acos(FVector::DotProduct({ 1,0,0 }, DirPlayer) / (1 * DirPlayer.Size())));
	if (DirPlayer.Y < 0) angle = -angle;
	float AnglePerAI = 360.f / AISquadList.Num();
	FormationPos.Empty();
	std::for_each(AISquadList.begin(), AISquadList.end(), [&](AAISquadController* BoidController) {
		FVector Pos{};
		FVector Offset{ 500.f, 0.f, 0.f };
		Offset = Offset.RotateAngleAxis(angle, { 0, 0, 1 });
		Offset = Offset.RotateAngleAxis(AnglePerAI * AISquadList.IndexOfByKey(BoidController), { 0, 0, 1 });
		Pos = LocPlayer + Offset;
		//DrawDebugPoint(Leader->GetWorld(), Pos, 20, FColor::Yellow);
		FormationPos.Add(Pos);
		});
}

void AAISquadManager::UpdateMission(const MissionType _MissionType, const FVector& _Location)
{
	Mission->Type = _MissionType;
	Mission->Location = _Location;

	for (AAISquadController* AISquad : AISquadList) {
		AISquad->SetMission(Mission);
		if(Mission->Type == MissionType::Formation){
			AISquad->get_blackboard()->SetValueAsBool("IsInFormation", true);
			AISquad->get_blackboard()->SetValueAsBool("HasOrder", false);
		}
		else if (Mission->Type != MissionType::None) {
			AISquad->get_blackboard()->SetValueAsBool("IsInFormation", false);
			AISquad->get_blackboard()->SetValueAsBool("HasOrder", true);
		}
	}

	// Sorry for that...
	switch (Mission->Type)
	{
	case MissionType::Defence:
		GEngine->AddOnScreenDebugMessage(4563, 4.f, FColor::Red, FString::Printf(TEXT("Order Defence on (%s,%s,%s) from %s"), *FString::SanitizeFloat(Mission->Location.X), *FString::SanitizeFloat(Mission->Location.Y), *FString::SanitizeFloat(Mission->Location.Z), *Leader->GetName()));
		break;
	case MissionType::Attack:
		GEngine->AddOnScreenDebugMessage(4563, 4.f, FColor::Red, FString::Printf(TEXT("Order Attack on (%s,%s,%s) from %s"), *FString::SanitizeFloat(Mission->Location.X), *FString::SanitizeFloat(Mission->Location.Y), *FString::SanitizeFloat(Mission->Location.Z), *Leader->GetName()));
		break;
	case MissionType::MoveTo:
		GEngine->AddOnScreenDebugMessage(4563, 4.f, FColor::Red, FString::Printf(TEXT("Order MoveTo on (%s,%s,%s) from %s"), *FString::SanitizeFloat(Mission->Location.X), *FString::SanitizeFloat(Mission->Location.Y), *FString::SanitizeFloat(Mission->Location.Z), *Leader->GetName()));
		break;
	default:
		GEngine->AddOnScreenDebugMessage(4563, 4.f, FColor::Red, FString::Printf(TEXT("Order Unknown on (%s,%s,%s) from %s"), *FString::SanitizeFloat(Mission->Location.X), *FString::SanitizeFloat(Mission->Location.Y), *FString::SanitizeFloat(Mission->Location.Z), *Leader->GetName()));
		break;
	}
}

// temp include, need to be replace by more robust code
void AAISquadManager::UpdateSquadTeam(TSubclassOf<ASoldierTeam> _NewTeam)
{
	Team = _NewTeam;
	for (auto SquadIA : AISquadList) {
		if (ASoldier* soldier = Cast<ASoldier>(SquadIA->GetPawn()); soldier) {
			soldier->SetTeam(_NewTeam);
		}
	}
}
