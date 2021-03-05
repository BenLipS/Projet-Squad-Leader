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
	DrawGrid();
}

void AInfluenceMapGrid::InitGrid() noexcept {

	for (int y = m_tile_height / 2.f; y <= m_grid_height; y += m_tile_height) {
		for (int x = m_tile_width / 2.f; x <= m_grid_width; x += m_tile_width) {
			FTileBase m_tile;
			if (auto _location = FVector(x, y, 20.f); IsValid(_location)) {
				m_tile.m_location = _location;
				m_influencemap.Add(m_tile);
			}
		}
	}
	nbr_tile = m_influencemap.Num();
}

bool AInfluenceMapGrid::IsValid(FVector _location) const {
	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), FVector(11740.f, 7030.f, 20.f), _location);
	return path->IsValid() && (!path->IsPartial());
}

void AInfluenceMapGrid::DrawGrid() const {
	for (FTileBase _tile : m_influencemap) {
		if(_tile.m_team == 1)
			DrawDebugSolidBox(GetWorld(),_tile.m_location, FVector(95.f, 95.f, 10.f), FColor(0, 0, 255));
		else if (_tile.m_team == 2)
			DrawDebugSolidBox(GetWorld(), _tile.m_location, FVector(95.f, 95.f, 10.f), FColor(255, 0, 0));
		else
			DrawDebugSolidBox(GetWorld(), _tile.m_location, FVector(95.f, 95.f, 10.f), FColor(0, 255, 0));
	}
}