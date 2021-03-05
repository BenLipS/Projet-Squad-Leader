// Fill out your copyright notice in the Description page of Project Settings.


#include "InfluenceMapGrid.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

AInfluenceMapGrid::AInfluenceMapGrid() {
	//Set this AInfo to be call every Tick. 
	PrimaryActorTick.bCanEverTick = true;
}

void AInfluenceMapGrid::BeginPlay() {
	Super::BeginPlay();
	InitGrid();
}

void AInfluenceMapGrid::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(100, 1.f, FColor::Black, TEXT("HELLO, i'm the new InfluenceMap :)"));*/
	
}

void AInfluenceMapGrid::InitGrid() noexcept {
	float _location_tile_x = m_tile_width / 2.f;
	float _location_tile_y = m_tile_height / 2.f;

	for (int y = 0; y != m_grid_height; y += m_grid_width) {
		for (int x = 0; x != m_grid_width; ++x) {
			FTileBase m_tile;
			m_tile.m_location = FVector(_location_tile_x, _location_tile_y, 10.f);
			m_influencemap.Add(m_tile);
			_location_tile_x += m_tile_width;
		}
		_location_tile_x = m_tile_width / 2.f;
		_location_tile_y += m_grid_width;
	}
}

bool AInfluenceMapGrid::IsValid(FVector _location) const {
	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), FVector(11740.f, 7030.f, 20.f), _location);
	return path->IsValid() && (!path->IsPartial());
}