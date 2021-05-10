#include "Shield.h"
#include "SquadLeader/SquadLeader.h"
#include "SquadLeader/Soldiers/Soldier.h"
#include "../SquadLeaderGameModeBase.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/GameplayStatics.h"

AShield::AShield()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AShield::BeginPlay()
{
	Super::BeginPlay();

	ASoldier* SourceSoldier = Cast<ASoldier>(GetOwner());
	SetTeam(SourceSoldier ? SourceSoldier->GetTeam() : nullptr);

	SetCollisionProfile(CollisionProfileNameMesh);

	UpdateMaterialColor();

	CreateInfluence();
}

void AShield::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShield, Health);
	DOREPLIFETIME(AShield, Team);
	DOREPLIFETIME(AShield, CollisionProfileNameMesh);
}

UStaticMeshComponent* AShield::GetMesh() const
{
	return Mesh;
}

void AShield::SetCollisionProfile(const FName& _Name)
{
	CollisionProfileNameMesh = _Name;
	Mesh->SetCollisionProfileName(CollisionProfileNameMesh);
}

FName AShield::GetCollisionProfile() const
{
	return CollisionProfileNameMesh;
}

void AShield::Destroyed()
{
	EraseInfluence();
	Super::Destroyed();
}

void AShield::SetHealth(const float _Health)
{
	Health = _Health;
}

void AShield::ApplyDamages(const float _Damage)
{
	if (GetLocalRole() < ROLE_Authority)
		ServerApplyDamages(_Damage);

	Health -= _Damage;
	if (Health <= 0.f)
		Destroy();
}

void AShield::ServerApplyDamages_Implementation(const float _Damage)
{
	ApplyDamages(_Damage);
}

bool AShield::ServerApplyDamages_Validate(const float _Damage)
{
	return true;
}

ASoldierTeam* AShield::GetTeam()
{
	return Team;
}

bool AShield::SetTeam(ASoldierTeam* _Team)
{
	if (GetLocalRole() < ROLE_Authority)
		return false;

	Team = _Team;
	return true;
}

void AShield::UpdateMaterialColor()
{
	if (!Mesh)
		return;

	UMaterialInstanceDynamic* ShieldMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), Mesh->GetMaterial(0));

	// Soldier that will see the shield in his world
	ASoldier* Soldier = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn<ASoldier>();
	// Soldier who casted the shield
	ASoldier* SoldierShieldOwner = Cast<ASoldier>(GetInstigator());

	if (!Soldier || !SoldierShieldOwner || Soldier->GetTeam() != SoldierShieldOwner->GetTeam())
		ShieldMaterialInstance->SetVectorParameterValue(ColorParameterName, ColorMeshEnnemy);
	else
		ShieldMaterialInstance->SetVectorParameterValue(ColorParameterName, ColorMeshAllie);

	Mesh->SetMaterial(0, ShieldMaterialInstance);
}

void AShield::CreateInfluence() {
	//GEngine->AddOnScreenDebugMessage(10, 1.0f, FColor::Purple, TEXT("Send Influence"));
	ASquadLeaderGameModeBase* GameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GetTeam() && GameMode && GameMode->InfluenceMap) {
		FGridPackageObstacle Package;
		Package.m_location_on_map = this->GetActorLocation();
		Package.team_value = GetTeam()->Id;
		Package.m_type = Type::Obstacle;
		Package.ActorID = this->GetUniqueID();
		GameMode->InfluenceMap->ReceivedMessage(Package);
	}
}

void AShield::EraseInfluence() {
	//GEngine->AddOnScreenDebugMessage(20, 1.0f, FColor::Purple, TEXT("Erase Influence"));
	ASquadLeaderGameModeBase* GameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GetTeam() && GameMode && GameMode->InfluenceMap) {
		FGridPackageObstacle Package;
		Package.m_location_on_map = this->GetActorLocation();
		Package.team_value = GetTeam()->Id;
		Package.m_type = Type::Obstacle;
		Package.ActorID = this->GetUniqueID();
		GameMode->InfluenceMap->EraseObstacleInfluence(Package);
	}
}