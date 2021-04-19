// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstacleActor.h"
#include "../../GameState/SquadLeaderGameState.h"
#include "../../SquadLeaderGameModeBase.h"

// Sets default values
AObstacleActor::AObstacleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AObstacleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObstacleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TickNumber % 2 == 0) {
		auto GM = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM && GM->InfluenceMap) {
			FGridPackage Package;
			Package.m_location_on_map = GetActorLocation();
			Package.team_value = 1;
			Package.m_type = Type::Projectile;
			GM->InfluenceMap->ReceivedMessage(Package);
		}
	}
	TickNumber++; 
}

