#include "Shield.h"
#include "SquadLeader/SquadLeader.h"
#include "SquadLeader/Soldiers/Soldier.h"

AShield::AShield() : Health { 100.f }
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Mesh->SetCollisionProfileName(TEXT("Shield")); // Shield is a predifined collision from the editor
}

void AShield::BeginPlay()
{
	Super::BeginPlay();

	ASoldier* SourceSoldier = Cast<ASoldier>(GetOwner());
	SetTeam(SourceSoldier ? SourceSoldier->GetTeam() : nullptr);
}

void AShield::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShield, Health);
	DOREPLIFETIME(AShield, Team);
}

void AShield::DestroyShield()
{
	Destroy();
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
	if (Health < 0.f)
		DestroyShield();
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