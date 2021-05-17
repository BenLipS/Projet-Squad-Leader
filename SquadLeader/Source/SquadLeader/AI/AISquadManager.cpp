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

#include "../SquadLeaderGameInstance.h"
#include "../MainMenu/GameParam/GameParam.h"
#include "../Soldiers/Soldier.h"
#include "../Soldiers/Players/SoldierPlayerState.h"
#include "../MainMenu/PlayerParam/PlayerParam.h"

#include<algorithm>

AAISquadManager::AAISquadManager()
{
	bReplicates = true;
	bAlwaysRelevant = true;

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}

void AAISquadManager::Init(ASoldierTeam* _Team, ASoldierPlayer* _Player)
{
	Team = _Team;
	Leader = _Player;

	// fetch information from the game mode if available (so server only)
	uint8 StartNumberOfSoldiers;
	if (GetLocalRole() == ROLE_Authority) {
		if (OverrideStartNumberOfSoldiers < 0) {
			// get game mode params directly from the source because "StartPlay" is not guaranteed to have been already called
			AGameParam* ImportedGameParam = GetGameInstance<USquadLeaderGameInstance>()->GameParam.GetDefaultObject();
			StartNumberOfSoldiers = ImportedGameParam->StartingNbAISquad;
		}
		else
			StartNumberOfSoldiers = OverrideStartNumberOfSoldiers;
	}
	else StartNumberOfSoldiers = 0;

	const FTransform PlayerTransform = Leader->GetTransform();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (uint8 i = 0; i < StartNumberOfSoldiers; ++i)
	{
		const float Angle = FMath::RadiansToDegrees(i * 2 * PI / StartNumberOfSoldiers);
		FTransform TransformAI = PlayerTransform;
		TransformAI.SetLocation(PlayerTransform.GetLocation() + FVector{ 300.f, 0.f, 0.f }.RotateAngleAxis(Angle, FVector{ 0.f, 0.f, 1.f }));

		TransformAI.SetScale3D(FVector::OneVector);

		ASoldierAI* SquadAI = GetWorld()->SpawnActorDeferred<ASoldierAI>(Leader->GetPlayerState<ASoldierPlayerState>()->GetPlayerParam()->GetAISoldier(i),
			TransformAI, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (SquadAI)
		{
			SquadAI->bUpdateTeamOnSpawn = false; // Wait for squadManager replication before updating the team
			SquadAI->SpawnDefaultController();
			SquadAI->OnHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberHealthChange);
			SquadAI->OnMaxHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxHealthChange);
			SquadAI->OnShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberShieldChange);
			SquadAI->OnMaxShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxShieldChange);

			if (AAISquadController* AC = Cast<AAISquadController>(SquadAI->Controller))
			{
				AISquadControllerList.Add(AC);
				AISoldierList.Add(SquadAI);
				AC->SquadManager = this;
			}
			SquadAI->FinishSpawning(TransformAI);
			SquadAI->BroadCastDatas();
		}
	}

	UpdateSquadTeam(Team);
	m_inFormation = true;
	TypeOfFormation = FormationType::Circle;
	BroadCastSquadData();
}

void AAISquadManager::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		ASoldierPlayer* SoldierPlayer = Cast<ASoldierPlayer>(GetOwner());
		ASoldierTeam* SoldierTeam = SoldierPlayer ? SoldierPlayer->GetTeam() : nullptr;

		if (SoldierTeam)
			Init(SoldierTeam, SoldierPlayer);
	}
}

void AAISquadManager::Tick(float DeltaTime)
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Tick for: %s"), *Leader->GetName()));

	//if (m_inFormation) {
	UpdateFormation();
	//}

	Super::Tick(DeltaTime);
}

void AAISquadManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAISquadManager, AISoldierList);
	DOREPLIFETIME(AAISquadManager, Leader);
}

TArray<ASoldierAI*> AAISquadManager::GetAISoldierList() const
{
	return AISoldierList;
}

void AAISquadManager::OnRep_AISoldierList()
{
	if (Leader)
		Leader->OnRep_SquadManager();
}

void AAISquadManager::AddAnAIToSquad_Implementation()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
		return;

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("AAAAAAAAAAAAAAAAAAAAAAAAA"));
	TSubclassOf<ASoldierAI> ClassAI;
	ClassAI = Leader->GetPlayerState<ASoldierPlayerState>()->GetPlayerParam()->GetAISoldier(AISquadControllerList.Num());

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
		SquadAI->bUpdateTeamOnSpawn = false; // Wait for squadManager replication before updating the team
		SquadAI->SpawnDefaultController();
		SquadAI->OnHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberHealthChange);
		SquadAI->OnMaxHealthChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxHealthChange);
		SquadAI->OnShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberShieldChange);
		SquadAI->OnMaxShieldChanged.AddDynamic(this, &AAISquadManager::OnSquadMemberMaxShieldChange);
		SquadAI->SetTeam(Team);

		if (AAISquadController* AC = Cast<AAISquadController>(SquadAI->Controller))
		{
			AISquadControllerList.Add(AC);
			AISoldierList.Add(SquadAI);
			AC->SquadManager = this;
		}
		SquadAI->FinishSpawning(LocationAI);
		SquadAI->BroadCastDatas();
		BroadCastSquadData();
	}
}

bool AAISquadManager::HasSoldier(ASoldier* _Soldier) const
{
	ASoldierAI* SoldierAI = Cast<ASoldierAI>(_Soldier);
	return AISoldierList.Find(SoldierAI) != INDEX_NONE;
}

void AAISquadManager::UpdateFormation()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
		return;

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

	for (int i = 0; i < AISquadControllerList.Num(); i++) {
		FVector FormationPosAI = FormationPos[i];
		FormationPosAI = UNavigationSystemV1::ProjectPointToNavigation(GetWorld(), FormationPosAI);
		AISquadControllerList[i]->UpdateFormation(FormationPosAI);
	}
}

void AAISquadManager::UpdateCircleFormation()
{
	FVector DirPlayer = Leader->GetActorForwardVector();
	DirPlayer = -DirPlayer;
	FVector LocPlayer = Leader->GetActorLocation();
	float angle = FMath::RadiansToDegrees(FGenericPlatformMath::Acos(FVector::DotProduct({ 1,0,0 }, DirPlayer) / (1 * DirPlayer.Size())));
	if (DirPlayer.Y < 0) angle = -angle;
	float AnglePerAI = 360.f / AISquadControllerList.Num();
	FormationPos.Empty();
	std::for_each(AISquadControllerList.begin(), AISquadControllerList.end(), [&](AAISquadController* BoidController) {
		FVector Pos{};
		FVector Offset{ 500.f, 0.f, 0.f };
		Offset = Offset.RotateAngleAxis(angle, { 0, 0, 1 });
		Offset = Offset.RotateAngleAxis(AnglePerAI * AISquadControllerList.IndexOfByKey(BoidController), { 0, 0, 1 });
		Pos = LocPlayer + Offset;
		FormationPos.Add(Pos);
		});
}

void AAISquadManager::UpdateArrowFormation()
{
	if (AISquadControllerList.Num() > 1) {
		FVector DirPlayer = Leader->GetActorForwardVector();
		//DirPlayer = -DirPlayer;
		FVector LocPlayer = Leader->GetActorLocation();
		float angle = FMath::RadiansToDegrees(FGenericPlatformMath::Acos(FVector::DotProduct({ 1,0,0 }, DirPlayer) / (1 * DirPlayer.Size())));
		if (DirPlayer.Y < 0) angle = -angle;
		float AnglePerAI = 180 / (AISquadControllerList.Num() - 1);
		FormationPos.Empty();
		std::for_each(AISquadControllerList.begin(), AISquadControllerList.end(), [&](AAISquadController* BoidController) {
			FVector Pos{};
			FVector Offset{ 0.f, -1.f, 0.f };
			Offset = Offset.RotateAngleAxis(angle, { 0, 0, 1 });
			Offset = Offset.RotateAngleAxis(AnglePerAI * AISquadControllerList.IndexOfByKey(BoidController), { 0, 0, 1 });
			float CoefToGetCloser = FMath::Abs(90 - AnglePerAI * AISquadControllerList.IndexOfByKey(BoidController)) / 90;
			Pos = LocPlayer + Offset * 200 * (1 - CoefToGetCloser + 1.5f);
			//DrawDebugPoint(Leader->GetWorld(), Pos, 20, FColor::Yellow);
			FormationPos.Add(Pos);
			});
	}
}

void AAISquadManager::UpdateMission(const MissionType _MissionType, const FVector& _Location)
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
		return;

	FVector ValidMissionLocation = GetValidMissionLocation(_Location);
	auto for_each_sqaud = [&](bool hasOrder, bool isInFormation) {
		for (AAISquadController* AISquad : AISquadControllerList) {

			UPatrolMission* _patrolMisssion = Cast<UPatrolMission>(NewObject<UPatrolMission>(this, UPatrolMission::StaticClass()));
			_patrolMisssion->InitPatrolMission(1, MissionPriority::eMIDDLE);
			AISquad->SetMission<UPatrolMission*>(_patrolMisssion);
			AISquad->SetObjectifLocation(ValidMissionLocation);
			
			AISquad->SetUpMission(true, false, ValidMissionLocation);
		}
	};

	switch (_MissionType)
	{
	case MissionType::FormationCircle:
		Leader->DestroyPing();
		for (AAISquadController* AISquad : AISquadControllerList) {
			AISquad->EmptyMissionList();
			UFormationMission* _formationMission = Cast<UFormationMission>(NewObject<UFormationMission>(this, UFormationMission::StaticClass()));
			_formationMission->InitFormation(1, MissionPriority::eBASIC, ValidMissionLocation);
			AISquad->SetMission<UFormationMission*>(_formationMission);
			TypeOfFormation = FormationType::Circle;
			
			AISquad->SetUpMission(false, true, ValidMissionLocation);
		}
		m_inFormation = true;
		break;
	case MissionType::FormationArrow:
		Leader->DestroyPing();
		for (AAISquadController* AISquad : AISquadControllerList) {
			AISquad->EmptyMissionList();
			UFormationMission* _formationMission = Cast<UFormationMission>(NewObject<UFormationMission>(this, UFormationMission::StaticClass()));
			_formationMission->InitFormation(1, MissionPriority::eBASIC, ValidMissionLocation);
			AISquad->SetMission<UFormationMission*>(_formationMission);
			TypeOfFormation = FormationType::Arrow;

			AISquad->SetUpMission(false, true, ValidMissionLocation);
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
		Leader->SpawnPing(ValidMissionLocation);
		//GEngine->AddOnScreenDebugMessage(4563, 4.f, FColor::Red, FString::Printf(TEXT("Order MoveTo on (%s,%s,%s) from %s"), *FString::SanitizeFloat(_Location.X), *FString::SanitizeFloat(_Location.Y), *FString::SanitizeFloat(_Location.Z), *Leader->GetName()));
		break;
	default:
		for_each_sqaud(true, false);
		m_inFormation = false;
		//GEngine->AddOnScreenDebugMessage(4563, 4.f, FColor::Red, FString::Printf(TEXT("Order Unknown on (%s,%s,%s) from %s"), *FString::SanitizeFloat(_Location.X), *FString::SanitizeFloat(_Location.Y), *FString::SanitizeFloat(_Location.Z), *Leader->GetName()));
		break;
	}
}

FVector AAISquadManager::GetValidMissionLocation(const FVector& _Location)
{
	return UNavigationSystemV1::ProjectPointToNavigation(GetWorld(), _Location, NULL, NULL, FVector{0.f,0.f,1500.f});
}

// temp include, need to be replace by more robust code
void AAISquadManager::UpdateSquadTeam(ASoldierTeam* _NewTeam)
{
	Team = _NewTeam;
	for (auto SquadIA : AISquadControllerList) {
		if (ASoldier* soldier = Cast<ASoldier>(SquadIA->GetPawn()); soldier) {
			soldier->SetTeam(_NewTeam);
		}
	}
}

void AAISquadManager::BroadCastSquadData()
{
	TArray<FSoldierAIData> SoldierData;
	for (auto AIC : AISquadControllerList)
	{
		FSoldierAIData data;
		if (ASoldierAI* SoldierAI = AIC->GetPawn<ASoldierAI>(); SoldierAI)
		{
			data.Health = SoldierAI->GetHealth();
			data.MaxHealth = SoldierAI->GetMaxHealth();
			data.Shield = SoldierAI->GetShield();
			data.MaxShield = SoldierAI->GetMaxShield();
			data.MissionState = AIC->GetState();
			data.ClassSoldier = AIC->GetClass();
			SoldierData.Add(data);
		}
	}
	OnSquadChanged.Broadcast(SoldierData);
}

void AAISquadManager::OnSquadMemberHealthChange(float newHealth, AAISquadController* SoldierController)
{
	int index;
	index = AISquadControllerList.Find(SoldierController);
	if (index != INDEX_NONE)
	{
		OnMemberHealthChanged.Broadcast(index, newHealth);
	}
}

void AAISquadManager::OnSquadMemberMaxHealthChange(float newMaxHealth, AAISquadController* SoldierController)
{
	int index;
	index = AISquadControllerList.Find(SoldierController);
	if (index != INDEX_NONE)
	{
		OnMemberMaxHealthChanged.Broadcast(index, newMaxHealth);
	}
}

void AAISquadManager::OnSquadMemberShieldChange(float newShield, AAISquadController* SoldierController)
{
	int index;
	index = AISquadControllerList.Find(SoldierController);
	if (index != INDEX_NONE)
	{
		OnMemberShieldChanged.Broadcast(index, newShield);
	}
}

void AAISquadManager::OnSquadMemberMaxShieldChange(float newMaxShield, AAISquadController* SoldierController)
{
	int index;
	index = AISquadControllerList.Find(SoldierController);
	if (index != INDEX_NONE)
	{
		OnMemberMaxShieldChanged.Broadcast(index, newMaxShield);
	}
}

void AAISquadManager::OnSquadMemberMissionChange(AIBasicState newValue, AAISquadController* SoldierController)
{
	int index;
	index = AISquadControllerList.Find(SoldierController);
	if (index != INDEX_NONE)
	{
		OnMemberStateChanged.Broadcast(index, newValue);
	}
}

void AAISquadManager::OnSquadMemberClassChange(SoldierClass newValue, AAISquadController* SoldierController)
{
	int index;
	index = AISquadControllerList.Find(SoldierController);
	if (index != INDEX_NONE)
	{
		OnMemberClassChanged.Broadcast(index, newValue);
	}
}

void FAISquadManagerData::OnSquadDataChanged(const TArray<FSoldierAIData>& newValue)
{
	SquadData = newValue;
}