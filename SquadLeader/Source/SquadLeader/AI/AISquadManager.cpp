// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquadManager.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"

#include<algorithm>
// temp include, need to be replace by more robust code
#include "../Soldiers/Soldier.h"

AAISquadManager::AAISquadManager() {
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}
void AAISquadManager::BeginPlay()
{
	Super::BeginPlay();
}

void AAISquadManager::Init(ASoldierTeam* _Team, ASoldierPlayer* _Player)
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
		SquadAI->OnHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberHealthChange);
		SquadAI->OnMaxHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxHealthChange);
		SquadAI->OnShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberShieldChange);
		SquadAI->OnMaxShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxShieldChange);
		SquadAI->FinishSpawning(LocationAI);
		SquadAI->BroadCastDatas();
		AISquadList.Add(Cast<AAISquadController>(SquadAI->Controller));
		Cast<AAISquadController>(SquadAI->Controller)->SquadManager = this;
	}
	ASoldierAI* SquadAI1 = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI1, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (SquadAI1) {
		SquadAI1->SpawnDefaultController();
		SquadAI1->OnHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberHealthChange);
		SquadAI1->OnMaxHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxHealthChange);
		SquadAI1->OnShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberShieldChange);
		SquadAI1->OnMaxShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxShieldChange);
		SquadAI1->FinishSpawning(LocationAI1);
		SquadAI1->BroadCastDatas();
		AISquadList.Add(Cast<AAISquadController>(SquadAI1->Controller));
		Cast<AAISquadController>(SquadAI1->Controller)->SquadManager = this;
	}
	ASoldierAI* SquadAI2 = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI2, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (SquadAI2) {
		SquadAI2->SpawnDefaultController();
		SquadAI2->OnHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberHealthChange);
		SquadAI2->OnMaxHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxHealthChange);
		SquadAI2->OnShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberShieldChange);
		SquadAI2->OnMaxShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxShieldChange);
		SquadAI2->FinishSpawning(LocationAI2);
		SquadAI2->BroadCastDatas();
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

	//if (Mission->Type == MissionType::Formation) {
	UpdateFormation();
	//}

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
		AISquad->SetObjectifLocation(_Location);
		AISquad->StopCurrentBehavior = true;
		if(Mission->Type == MissionType::Formation){
			AISquad->FormationState();
			AISquad->get_blackboard()->SetValueAsBool("HasOrder", false);
		}
		else if (Mission->Type != MissionType::None) {
			AISquad->SetState(AIBasicState::Moving);
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
void AAISquadManager::UpdateSquadTeam(ASoldierTeam* _NewTeam)
{
	Team = _NewTeam;
	for (auto SquadIA : AISquadList) {
		if (ASoldier* soldier = Cast<ASoldier>(SquadIA->GetPawn()); soldier) {
			soldier->SetTeam(_NewTeam);
		}
	}
}

void AAISquadManager::BroadCastSquadData()
{
	TArray<FSoldierAIData> SoldierData;
	for (auto AIC : AISquadList)
	{
		FSoldierAIData data;
		if (ASoldierAI* SoldierAI = AIC->GetPawn<ASoldierAI>(); SoldierAI)
		{
			data.Health = SoldierAI->GetHealth();
			data.MaxHealth = SoldierAI->GetMaxHealth();
			data.Shield = SoldierAI->GetShield();
			data.MaxShield = SoldierAI->GetMaxShield();
			SoldierData.Add(data);
		}
	}
	OnSquadChanged.Broadcast(SoldierData);
}

void AAISquadManager::OnSquadMemberHealthChange(float newHealth, AAISquadController* SoldierController)
{
	int index;
	index = AISquadList.Find(SoldierController);
	if (index != INDEX_NONE)
	{
		OnMemberHealthChanged.Broadcast(index, newHealth);
	}
}

void AAISquadManager::OnSquadMemberMaxHealthChange(float newMaxHealth, AAISquadController* SoldierController)
{
	int index;
	index = AISquadList.Find(SoldierController);
	if (index != INDEX_NONE)
	{
		OnMemberMaxHealthChanged.Broadcast(index, newMaxHealth);
	}
}

void AAISquadManager::OnSquadMemberShieldChange(float newShield, AAISquadController* SoldierController)
{
	int index;
	index = AISquadList.Find(SoldierController);
	if (index != INDEX_NONE)
	{
		OnMemberShieldChanged.Broadcast(index, newShield);
	}
}

void AAISquadManager::OnSquadMemberMaxShieldChange(float newMaxShield, AAISquadController* SoldierController)
{
	int index;
	index = AISquadList.Find(SoldierController);
	if (index != INDEX_NONE)
	{
		OnMemberMaxShieldChanged.Broadcast(index, newMaxShield);
	}
}

void FAISquadManagerData::OnSquadDataChanged(const TArray<FSoldierAIData>& newValue)
{
	SquadData = newValue;
}