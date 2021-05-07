#include "SoldierSecondarySpawn.h"

ASoldierSecondarySpawn::ASoldierSecondarySpawn() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}


void ASoldierSecondarySpawn::BeginPlay() {
	Super::BeginPlay();
}


void ASoldierSecondarySpawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}