#include "Shield.h"
#include "SquadLeader/SquadLeader.h"

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
}

void AShield::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShield, Health);
}

void AShield::DestroyShield()
{
	Destroy();
}

void AShield::SetHealth(const float _Health)
{
	Health = _Health;
}

void AShield::OnReceiveDamage(const float _Damage)
{
	if (GetLocalRole() < ROLE_Authority)
		ServerOnReceiveDamage(_Damage);

	Health -= _Damage;
	if (Health < 0.f)
		DestroyShield();
}

void AShield::ServerOnReceiveDamage_Implementation(const float _Damage)
{
	OnReceiveDamage(_Damage);
}

bool AShield::ServerOnReceiveDamage_Validate(const float _Damage)
{
	return true;
}