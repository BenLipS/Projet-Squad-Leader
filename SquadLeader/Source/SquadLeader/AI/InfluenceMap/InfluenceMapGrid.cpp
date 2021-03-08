// Fill out your copyright notice in the Description page of Project Settings.


#include "InfluenceMapGrid.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "../../SquadLeaderGameModeBase.h"
#include "../../Soldiers/Soldier.h"

AInfluenceMapGrid::AInfluenceMapGrid() {
	//Set this AInfo to be call every Tick. 
	PrimaryActorTick.bCanEverTick = true;
}

void AInfluenceMapGrid::BeginPlay() {
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority) {
		InitGrid();
		m_neighboors.Init(FNeighboor(), nbr_tile);
		FindNeighboor();
	}
}

void AInfluenceMapGrid::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(100, 1.f, FColor::Black, TEXT("HELLO, i'm the new InfluenceMap :)"));*/
	if (GetLocalRole() == ROLE_Authority) {
		//DrawGrid();
		ResetGrid();
		UpdateGrid();
	}
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
	UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), FVector(5000.f, 5000.f, 80.f), _location);
	return path->IsValid() && (!path->IsPartial());
}

void AInfluenceMapGrid::DrawGrid() const {
	for (FTileBase _tile : m_influencemap) {
		if(_tile.m_team == 1)
			DrawDebugSolidBox(GetWorld(),_tile.m_location, FVector(95.f, 95.f,10.f), FColor(0, 0, 255 * _tile.m_value));
		else if (_tile.m_team == 2)
			DrawDebugSolidBox(GetWorld(), _tile.m_location, FVector(95.f, 95.f, 10.f), FColor(255 * _tile.m_value, 0, 0));
		/*else
			DrawDebugSolidBox(GetWorld(), _tile.m_location, FVector(95.f, 95.f, 10.f), FColor(0, 255, 0));*/
	}
}

void AInfluenceMapGrid::ResetGrid() noexcept {
	for (FTileBase& _tile : m_influencemap) {
		_tile.m_value = 0.f;
		_tile.m_team = -1;
	}
}

void AInfluenceMapGrid::UpdateGrid() noexcept {
	UpdatePlayers();
	UpdateControlArea();
}

int AInfluenceMapGrid::FindTileIndex(FVector _location) const noexcept {
	bool result = false;
	for (int i = 0; i != nbr_tile; ++i) {
		result = IsOnTile(_location, m_influencemap[i].m_location);
		if (result)
			return i;
	}
	return -1;
}

bool AInfluenceMapGrid::IsOnTile(FVector _location, FVector tile_location) const noexcept {
	float width = m_tile_width / 2.f;
	float height = m_tile_height / 2.f;
	bool on_axis_x = _location.X <= tile_location.X + width && _location.X >= tile_location.X - width;
	bool on_axis_y = _location.Y <= tile_location.Y + height && _location.Y >= tile_location.Y - height;

	return on_axis_x && on_axis_y;
}

void AInfluenceMapGrid::Neighboors(int index) noexcept {
	int index_up = FindTileIndex(FVector(m_influencemap[index].m_location.X, m_influencemap[index].m_location.Y + m_tile_height, m_influencemap[index].m_location.Z));
	int index_down = FindTileIndex(FVector(m_influencemap[index].m_location.X, m_influencemap[index].m_location.Y - m_tile_width, m_influencemap[index].m_location.Z));
	int index_left = index - 1;
	int index_right = index + 1;

	if (index_up != -1 && index_up < nbr_tile) 
		IsNeighboor(index, index_up);
	if (index_right < nbr_tile)
		IsNeighboor(index, index_right);
		
	if (index_down != -1 && index_down >= 0) 
		IsNeighboor(index, index_down);
	if (index_left >= 0)
		IsNeighboor(index, index_left);
}

void AInfluenceMapGrid::IsNeighboor(int index_1, int index_2) noexcept {
	int distance = FVector::Dist(m_influencemap[index_1].m_location, m_influencemap[index_2].m_location);
	if (distance <= 200.f) {
		m_neighboors[index_1].m_neighboor.Add(index_2);
	}
}

void AInfluenceMapGrid::FindNeighboor()noexcept {
	for (int i = 0; i != nbr_tile; ++i)
		Neighboors(i);
}

void AInfluenceMapGrid::Influence(int index, int start_index, int source_index, int distance) noexcept {
	float value = m_influencemap[source_index].m_value / FMath::Sqrt(1.f + distance);

	if (distance > 5)
		return;

	for (int neighboor : m_neighboors[index].m_neighboor) {
		if (neighboor != start_index && m_influencemap[neighboor].m_value <= value) {
			/*if(m_influencemap[neighboor].m_value + value > 0.7f)
				m_influencemap[neighboor].m_value = 0.7f;
			else
				m_influencemap[neighboor].m_value += value;*/
			m_influencemap[neighboor].m_value = value;
			m_influencemap[neighboor].m_team = m_influencemap[index].m_team;
			Influence(neighboor, index, source_index, distance + 1);
		}
	}
}

void AInfluenceMapGrid::UpdatePlayers() noexcept {
	int team = 1;
	auto gamemode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
	for (TSubclassOf<ASoldierTeam> _team : gamemode->SoldierTeamCollection) {

		ASoldierTeam* _teamclass = Cast<ASoldierTeam>(_team->GetDefaultObject());

		for (ASoldier* _soldier : _teamclass->soldierList) {

			int index_tile = FindTileIndex(_soldier->GetLocation());
			if (index_tile != -1) {
				m_influencemap[index_tile].m_value = 0.7f;
				m_influencemap[index_tile].m_team = team;
				Influence(index_tile, index_tile, index_tile, 1);
			}
		}

		team = 2;
	}
}

void AInfluenceMapGrid::UpdateControlArea() noexcept {
	auto gamemode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
	AControlAreaManager* controlareamanager = Cast<AControlAreaManager>(gamemode->ControlAreaManager->GetDefaultObject());

	for (int i = 0; i != controlareamanager->GetControlArea().Num(); ++i) {
		AControlArea* _controlArea = Cast<AControlArea>(controlareamanager->GetControlArea()[i]);
		if (_controlArea->isTakenBy) {
			int index_tile = FindTileIndex(controlareamanager->GetControlArea()[i]->GetActorLocation());
			if (index_tile != -1) {
				m_influencemap[index_tile].m_value = 1.f;
				ASoldierTeam* _soldierTeam = Cast<ASoldierTeam>(_controlArea->isTakenBy->GetDefaultObject());
				if (_soldierTeam->TeamName == "Team1")
					m_influencemap[index_tile].m_team = 1;
				else 
					m_influencemap[index_tile].m_team = 2;
				Influence(index_tile, index_tile, index_tile, 1);
			}
		}
	}
}