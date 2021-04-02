#include "Shield.h"
#include "SquadLeader/SquadLeader.h"

AShield::AShield()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Mesh->SetCollisionProfileName(TEXT("Shield")); // Soldier is a predifined collision from the editor
}

void AShield::BeginPlay()
{
	Super::BeginPlay();
}

