// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquadManager.h"
#include "DrawDebugHelpers.h"
#include "Mission/FormationMission.h"
#include "Mission/DefendMission.h"
#include "Mission/CaptureMission.h"
#include "AISquadController.h"
#include "Mission/PatrolMission.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

#include<algorithm>
// temp include, need to be replace by more robust code
#include "../Soldiers/Soldier.h"

AAISquadManager::AAISquadManager() {
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}

void AAISquadManager::Init(ASoldierTeam* _Team, ASoldierPlayer* _Player)
{
	Team = _Team;
	Leader = _Player;

	const FTransform PlayerTransform = Leader->GetTransform();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (uint8 i = 0; i < StartNumberOfSoldiers; ++i)
	{
		const float Angle = FMath::RadiansToDegrees(i * 2 * PI / StartNumberOfSoldiers);
		FTransform TransformAI = PlayerTransform;
		TransformAI.SetLocation(PlayerTransform.GetLocation() + FVector{ 300.f, 0.f, 0.f }.RotateAngleAxis(Angle, FVector{ 0.f, 0.f, 1.f }));

		// TODO: Spawn all the classAIs. Perhaps use a TArray
		ASoldierAI* SquadAI = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI1, TransformAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (SquadAI)
		{
			SquadAI->SpawnDefaultController();
			SquadAI->OnHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberHealthChange);
			SquadAI->OnMaxHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxHealthChange);
			SquadAI->OnShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberShieldChange);
			SquadAI->OnMaxShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxShieldChange);
			SquadAI->FinishSpawning(TransformAI);
			SquadAI->BroadCastDatas();
			AISquadList.Add(Cast<AAISquadController>(SquadAI->Controller));
			Cast<AAISquadController>(SquadAI->Controller)->SquadManager = this;
		}
	}

	m_inFormation = true;
	TypeOfFormation = FormationType::Circle;
}

void AAISquadManager::Tick(float DeltaTime)
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Tick for: %s"), *Leader->GetName()));

	//if (m_inFormation) {
	UpdateFormation();
	//}

	Super::Tick(DeltaTime);
}

void AAISquadManager::AddAnAIToSquad_Implementation()
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("AAAAAAAAAAAAAAAAAAAAAAAAA"));
	TSubclassOf<ASoldierAI> ClassAI;
	switch (AISquadList.Num()) {
	case 0:
		ClassAI = ClassAI1;
		break;
	case 1:
		ClassAI = ClassAI2;
		break;
	case 2:
		ClassAI = ClassAI3;
		break;
	case 3:
		ClassAI = ClassAI4;
		break;
	case 4:
		ClassAI = ClassAI5;
		break;
	default:
		ClassAI = ClassAI1;
	}
	FTransform PlayerTransform = Leader->GetTransform();
	FTransform LocationAI;
	LocationAI.SetLocation(PlayerTransform.GetLocation() - Leader->GetActorForwardVector() * 500);

	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	FVector HitLocation{};

	if (navSys->NavigationRaycast(GetWorld(), PlayerTransform.GetLocation(), LocationAI.GetLocation(), HitLocation)) {
		LocationAI.SetLocation(HitLocation);
	}

	ASoldierAI* SquadAI = GetWorld()->SpawnActorDeferred<ASoldierAI>(ClassAI, LocationAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (SquadAI) {
		SquadAI->SpawnDefaultController();
		SquadAI->OnHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberHealthChange);
		SquadAI->OnMaxHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxHealthChange);
		SquadAI->OnShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberShieldChange);
		SquadAI->OnMaxShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxShieldChange);
		SquadAI->FinishSpawning(LocationAI);
		SquadAI->BroadCastDatas();
		SquadAI->SetTeam(Team);
		AISquadList.Add(Cast<AAISquadController>(SquadAI->Controller));
		Cast<AAISquadController>(SquadAI->Controller)->SquadManager = this;

		BroadCastSquadData();
	}
}

bool AAISquadManager::HasSoldier(const ASoldier* _Soldier) const
{
	if (AAISquadController* SC = Cast<AAISquadController>(_Soldier->GetController()))
		return AISquadList.Find(SC) != INDEX_NONE;
	return false;
}

void AAISquadManager::UpdateFormation()
{
	switch (TypeOfFormation) {
	case FormationType::Circle:
		UpdateCircleFormation();
		break;
	case FormationType::Arrow:
		UpdateArrowFormation();
		break;
	default: 
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Wrong Formation Type"));
	}

	for (int i = 0; i < AISquadList.Num(); i++) {
		FVector FormationPosAI = FormationPos[i];
		FormationPosAI = UNavigationSystemV1::ProjectPointToNavigation(GetWorld(), FormationPosAI);
		AISquadList[i]->UpdateFormation(FormationPosAI);
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

void AAISquadManager::UpdateArrowFormation()
{
	if (AISquadList.Num() > 1) {
		FVector DirPlayer = Leader->GetActorForwardVector();
		//DirPlayer = -DirPlayer;
		FVector LocPlayer = Leader->GetActorLocation();
		float angle = FMath::RadiansToDegrees(FGenericPlatformMath::Acos(FVector::DotProduct({ 1,0,0 }, DirPlayer) / (1 * DirPlayer.Size())));
		if (DirPlayer.Y < 0) angle = -angle;
		float AnglePerAI = 180 / (AISquadList.Num() - 1);
		FormationPos.Empty();
		std::for_each(AISquadList.begin(), AISquadList.end(), [&](AAISquadController* BoidController) {
			FVector Pos{};
			FVector Offset{ 0.f, -1.f, 0.f };
			Offset = Offset.RotateAngleAxis(angle, { 0, 0, 1 });
			Offset = Offset.RotateAngleAxis(AnglePerAI * AISquadList.IndexOfByKey(BoidController), { 0, 0, 1 });
			float CoefToGetCloser = FMath::Abs(90 - AnglePerAI * AISquadList.IndexOfByKey(BoidController)) / 90;
			Pos = LocPlayer + Offset * 200 * (1 - CoefToGetCloser + 1.5f);
			//DrawDebugPoint(Leader->GetWorld(), Pos, 20, FColor::Yellow);
			FormationPos.Add(Pos);
			});
	}
}

void AAISquadManager::UpdateMission(const MissionType _MissionType, const FVector& _Location)
{
	auto for_each_sqaud = [&](bool hasOrder, bool isInFormation) {
		for (AAISquadController* AISquad : AISquadList) {

			UPatrolMission* _patrolMisssion = Cast<UPatrolMission>(NewObject<UPatrolMission>(this, UPatrolMission::StaticClass()));
			_patrolMisssion->InitPatrolMission(1, MissionPriority::eMIDDLE);
			AISquad->SetMission<UPatrolMission*>(_patrolMisssion);
			AISquad->SetObjectifLocation(_Location);

			AISquad->SetUpMission(true, false, _Location);
		}
	};

	switch (_MissionType)
	{
	case MissionType::FormationCircle:

		for (AAISquadController* AISquad : AISquadList) {
			AISquad->EmptyMissionList();
			UFormationMission* _formationMission = Cast<UFormationMission>(NewObject<UFormationMission>(this, UFormationMission::StaticClass()));
			_formationMission->InitFormation(1, MissionPriority::eBASIC, _Location);
			AISquad->SetMission<UFormationMission*>(_formationMission);
			TypeOfFormation = FormationType::Circle;
			
			AISquad->SetUpMission(false, true, _Location);
		}
		m_inFormation = true;
		break;
	case MissionType::FormationArrow:

		for (AAISquadController* AISquad : AISquadList) {
			AISquad->EmptyMissionList();
			UFormationMission* _formationMission = Cast<UFormationMission>(NewObject<UFormationMission>(this, UFormationMission::StaticClass()));
			_formationMission->InitFormation(1, MissionPriority::eBASIC, _Location);
			AISquad->SetMission<UFormationMission*>(_formationMission);
			TypeOfFormation = FormationType::Arrow;

			AISquad->SetUpMission(false, true, _Location);
		}
		m_inFormation = true;
		break;
	case MissionType::eDEFEND:
		for_each_sqaud(true, false);
		m_inFormation = false;
		//GEngine->AddOnScreenDebugMessage(4563, 4.f, FColor::Red, FString::Printf(TEXT("Order Defence on (%s,%s,%s) from %s"), *FString::SanitizeFloat(_Location.X), *FString::SanitizeFloat(_Location.Y), *FString::SanitizeFloat(_Location.Z), *Leader->GetName()));
		break;
	case MissionType::eATTACK:
		for_each_sqaud(true, false);
		m_inFormation = false;
		//GEngine->AddOnScreenDebugMessage(4563, 4.f, FColor::Red, FString::Printf(TEXT("Order Attack on (%s,%s,%s) from %s"), *FString::SanitizeFloat(_Location.X), *FString::SanitizeFloat(_Location.Y), *FString::SanitizeFloat(_Location.Z), *Leader->GetName()));
		break;
	case MissionType::ePATROL:
		for_each_sqaud(true, false);
		m_inFormation = false;
		//GEngine->AddOnScreenDebugMessage(4563, 4.f, FColor::Red, FString::Printf(TEXT("Order MoveTo on (%s,%s,%s) from %s"), *FString::SanitizeFloat(_Location.X), *FString::SanitizeFloat(_Location.Y), *FString::SanitizeFloat(_Location.Z), *Leader->GetName()));
		break;
	default:
		for_each_sqaud(true, false);
		m_inFormation = false;
		//GEngine->AddOnScreenDebugMessage(4563, 4.f, FColor::Red, FString::Printf(TEXT("Order Unknown on (%s,%s,%s) from %s"), *FString::SanitizeFloat(_Location.X), *FString::SanitizeFloat(_Location.Y), *FString::SanitizeFloat(_Location.Z), *Leader->GetName()));
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