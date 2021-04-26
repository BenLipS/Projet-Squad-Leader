// Fill out your copyright notice in the Description page of Project Settings.


#include "InfluenceMapGrid.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "../../GameState/SquadLeaderGameState.h"
#include "../../Soldiers/Soldier.h"
#include "Templates/Function.h"
#include "EngineUtils.h"
#include "../../ControlArea/ControlAreaManager.h"


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
	if (GetLocalRole() == ROLE_Authority) {
		DrawGrid();
		{
			const double startTime = FPlatformTime::Seconds();

			// code de la fonction
			UpdateGrid();
			const double endTime = FPlatformTime::Seconds();
			const double elapsedTime = endTime - startTime;
			//GEngine->AddOnScreenDebugMessage(20, 2.f, FColor::Green, FString::Printf(TEXT("Ex?cution a dur? % .0fms."), elapsedTime * 1000));
		}
		value_tick++;
	}
}

void AInfluenceMapGrid::InitGrid() noexcept {
	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	auto box_ = navSys->GetLevelBounds(GetWorld()->GetCurrentLevel());

	for (int y = m_tile_height / 2.f; y <= m_grid_height; y += m_tile_height) {
		for (int x = m_tile_width / 2.f; x <= m_grid_width; x += m_tile_width) {
			FTileBase m_tile;
			auto _location = FVector(x, y, m_startLocation.Z);
			if (IsValid(_location)) {
				_location.Z += m_height;
				m_tile.m_location = _location;
				m_influencemap.Add(m_tile);
			}
		}
	}
	nbr_tile = m_influencemap.Num();
}

bool AInfluenceMapGrid::IsValid(FVector _location) const {
	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	UNavigationPath* path;
	for (float _height : m_heightList) {
		_location.Z = _height;
		path = navSys->FindPathToLocationSynchronously(GetWorld(), m_startLocation, _location);
		if (path->IsValid() && (!path->IsPartial()))
			return true;
	}
	return false;
}

void AInfluenceMapGrid::DrawGrid() const {
	if (m_DrawCharacterInfluence) {
		for (FTileBase _tile : m_influencemap) {
			if(_tile.m_type == Type::Projectile)
				DrawDebugSolidBox(GetWorld(), _tile.m_location, FVector(95.f, 95.f, 10.f), FColor(255 * _tile.m_value, 0, 255 * _tile.m_value));
			else if (_tile.m_team == 1)
				DrawDebugSolidBox(GetWorld(), _tile.m_location, FVector(95.f, 95.f, 10.f), FColor(0, 0, 255 * _tile.m_value));
			else if (_tile.m_team == 2)
				DrawDebugSolidBox(GetWorld(), _tile.m_location, FVector(95.f, 95.f, 10.f), FColor(255 * _tile.m_value, 0, 0));
			else if(m_DrawAllGrid)
				DrawDebugSolidBox(GetWorld(), _tile.m_location, FVector(95.f, 95.f, 10.f), FColor(0, 255, 0));
		}
	}
}

void AInfluenceMapGrid::UpdateGrid() noexcept {

	for (int32 index = m_index_update.Num() - 1; index >= 0; --index) {
		int index_tile = m_index_update[index];
		if (m_influencemap[index_tile].m_type != Type::ControlArea) {
			if (m_influencemap[index_tile].m_value >= 0.05f)
				m_influencemap[index_tile].m_value -= 0.05f;
			else {
				m_influencemap[index_tile].m_team = -1;
				m_influencemap[index_tile].in_update = false;
				m_influencemap[index_tile].m_type = Type::None;
			}
		}
	}

	for (int32 index = m_index_team1.Num() - 1; index >= 0; --index) {
		int index_tile = m_index_team1[index];
		if (m_influencemap[index_tile].m_type != Type::ControlArea) {
			/*if (m_influencemap[index_tile].m_value >= 0.05f)
				m_influencemap[index_tile].m_value -= 0.05f;
			else {
				m_influencemap[index_tile].m_team = -1;
				m_influencemap[index_tile].in_update = false;
				m_influencemap[index_tile].m_type = Type::None;
			}*/

			m_influencemap[index_tile].m_value = 0.f;
			m_influencemap[index_tile].m_team = -1;
			m_influencemap[index_tile].in_update = false;
			m_influencemap[index_tile].m_type = Type::None;
			m_influencemap[index_tile].ActorsID.Empty();
		}
	}

	for (int32 index = m_index_team2.Num() - 1; index >= 0; --index) {
		int index_tile = m_index_team2[index];
		if (m_influencemap[index_tile].m_type != Type::ControlArea) {
			/*if (m_influencemap[index_tile].m_value >= 0.05f)
				m_influencemap[index_tile].m_value -= 0.05f;
			else {
				m_influencemap[index_tile].m_team = -1;
				m_influencemap[index_tile].in_update = false;
				m_influencemap[index_tile].m_type = Type::None;
			}*/
			m_influencemap[index_tile].m_value = 0.f;
			m_influencemap[index_tile].m_team = -1;
			m_influencemap[index_tile].in_update = false;
			m_influencemap[index_tile].m_type = Type::None;
			m_influencemap[index_tile].ActorsID.Empty();
		}
	}

	auto functor = [&](const int index) { return !m_influencemap[index].in_update; };
	m_index_update.RemoveAll(functor);
	m_index_team1.RemoveAll(functor);
	m_index_team2.RemoveAll(functor);
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

bool AInfluenceMapGrid::FindIndexModifyinTeam1(const FVector2D Location, uint32& Index){
	if (m_index_team1.Num() > 0) {
		int begin = 0;
		int end = m_index_team1.Num() - 1;
		int middle = begin;
		bool find = false;

		while (!find && begin <= end) {
			middle = (begin + end) / 2;
			const FTileBase Tile = m_influencemap[m_index_team1[middle]];
			if (IsOnTileUpdate(Location, Tile.m_location))
				find = true;
			else if (Location.Y >= Tile.m_location.Y - (m_tile_height / 2.0f) && Location.Y <= Tile.m_location.Y + (m_tile_height / 2.f))
				if (Location.X > Tile.m_location.X)
					begin = middle + 1;
				else
					end = middle - 1;
			else
				if (Location.Y > Tile.m_location.Y - (m_tile_height / 2.0f))
					begin = middle + 1;
				else
					end = middle - 1;
		}

		if (find) {
			Index = m_index_team1[middle];
			return true;
		}
		return false;
	}
	return false;
}

bool AInfluenceMapGrid::FindIndexModifyinTeam2(const FVector2D Location, uint32& Index) {
	if (m_index_team2.Num() > 0) {
		int begin = 0;
		int end = m_index_team2.Num() - 1;
		int middle = begin;
		bool find = false;

		while (!find && begin <= end) {
			middle = (begin + end) / 2;
			const FTileBase Tile = m_influencemap[m_index_team2[middle]];
			if (IsOnTileUpdate(Location, Tile.m_location))
				find = true;
			else if (Location.Y >= Tile.m_location.Y - (m_tile_height / 2.0f) && Location.Y <= Tile.m_location.Y + (m_tile_height / 2.f))
				if (Location.X > Tile.m_location.X)
					begin = middle + 1;
				else
					end = middle - 1;
			else
				if (Location.Y > Tile.m_location.Y - (m_tile_height / 2.0f))
					begin = middle + 1;
				else
					end = middle - 1;
		}

		if (find) {
			Index = m_index_team2[middle];
			return true;
		}
		return false;
	}
	return false;
}

bool AInfluenceMapGrid::IsOnTile(FVector _location, FVector tile_location) const noexcept {
	float width = m_tile_width / 2.f;
	float height = m_tile_height / 2.f;
	bool on_axis_x = _location.X <= tile_location.X + width && _location.X >= tile_location.X - width;
	bool on_axis_y = _location.Y <= tile_location.Y + height && _location.Y >= tile_location.Y - height;

	return on_axis_x && on_axis_y;
}

bool AInfluenceMapGrid::IsOnTileUpdate(const FVector2D Location, const FVector TileLocation) const noexcept{
	const float width = m_tile_width / 2.f;
	const float height = m_tile_height / 2.f;

	const bool AxisX = (Location.X <= TileLocation.X + width) && (Location.X >= TileLocation.X - width);
	const bool AxisY = (Location.Y <= TileLocation.Y + height) && (Location.Y >= TileLocation.Y - height);

	return AxisX && AxisY;
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

void AInfluenceMapGrid::InfluenceSoldier(int index, int start_index, int source_index, int distance, uint32 actorID) noexcept {
	if (distance > CharacterAreaInfluence)
		return;

	for (int neighboor : m_neighboors[index].m_neighboor) {
		if (neighboor != start_index) {
			UpdateTile(neighboor, CharacterInfluence, m_influencemap[index].m_team, Type::Soldier, actorID);
			InfluenceSoldier(neighboor, index, source_index, distance + 1, actorID);
		}
	}
}

void AInfluenceMapGrid::InfluenceControlArea(int index, int start_index, int source_index, int distance, uint32 actorID) noexcept {
	if (distance > ControlAreaAreaInfluence)
		return;

	for (int neighboor : m_neighboors[index].m_neighboor) {
		if (neighboor != start_index) {
			UpdateTile(neighboor, ControlAreaInfluence, m_influencemap[index].m_team, Type::ControlArea, actorID);
			InfluenceControlArea(neighboor, index, source_index, distance + 1, actorID);
		}
	}
}

void AInfluenceMapGrid::InfluenceProjectile(int index, int start_index, int source_index, int distance, uint32 actorID) noexcept {

	if (distance > ProjectileAreaInfluence)
		return;

	for (int neighboor : m_neighboors[index].m_neighboor) {
		if (neighboor != start_index) {
			UpdateTile(neighboor, ProjectileInfluence, m_influencemap[index].m_team, Type::Projectile, actorID);
			InfluenceProjectile(neighboor, index, source_index, distance + 1, actorID);
		}
	}
}

void AInfluenceMapGrid::ReceivedMessage(FGridPackage _message) {
	int index_tile = FindTileIndex(_message.m_location_on_map);
	
	if (index_tile != -1) {
		switch (_message.m_type) {
		case Type::Soldier:
			UpdateTile(index_tile, CharacterInfluence, _message.team_value, _message.m_type, _message.ActorID);
			InfluenceSoldier(index_tile, index_tile, index_tile, 1, _message.ActorID);
			break;
		case Type::ControlArea:
			UpdateTile(index_tile, ControlAreaInfluence, _message.team_value, _message.m_type, _message.ActorID);
			InfluenceControlArea(index_tile, index_tile, index_tile, 1, _message.ActorID);
			break;
		case Type::Projectile:
			UpdateTile(index_tile, ProjectileInfluence, _message.team_value, _message.m_type, _message.ActorID);
			InfluenceProjectile(index_tile, index_tile, index_tile, 1, _message.ActorID);
			break;
		default:
			break;
		}
	}
}

void AInfluenceMapGrid::TimeFunction() {
	const double startTime = FPlatformTime::Seconds();

	// code de la fonction

	const double endTime = FPlatformTime::Seconds();
	const double elapsedTime = endTime - startTime;
}

void AInfluenceMapGrid::UpdateTile(int index, float value, int team, Type type, uint32 actorID) noexcept {
	auto bool1 = m_influencemap[index].in_update;
	auto bool2 = m_influencemap[index].ActorsID.Contains(actorID);


	if (!bool2) {
		
		if (m_influencemap[index].m_value + value <= 1.f)
			m_influencemap[index].m_value += value;
		else
			m_influencemap[index].m_value = 1.f;
		
		//m_index_update.Add(index);
		if (type != Type::Projectile) {
			if (team == 1)
				AddUpdateTileTeam1(index);
			else
				AddUpdateTileTeam2(index);
		}
		else {
			AddUpdateTileTeam1(index);
			AddUpdateTileTeam2(index);
		}
		m_influencemap[index].ActorsID.Add(actorID);
		m_influencemap[index].in_update = true;
	}

	m_influencemap[index].m_team = team;
	m_influencemap[index].m_type = type;
}

float AInfluenceMapGrid::GetValue(const FVector2D Location, const uint8 Team) {
	uint32 index = -1;
	float Cost = 1.f;
	if (Team == 1) {
		if (FindIndexModifyinTeam1(Location, index)) {
			Cost += m_influencemap[index].m_value * 10.f;
		}
	}
	else {
		if (FindIndexModifyinTeam2(Location, index)) {
			Cost += m_influencemap[index].m_value * 10.f;
		}
	}
	return Cost;
}

void AInfluenceMapGrid::AddUpdateTileTeam1(const uint32 index) {
	for (int i = 0; i != m_index_team1.Num(); ++i) {
		if (m_index_team1[i] >= index) {
			m_index_team1.Insert(index, i);
			return;
		}
	}
	m_index_team1.Add(index);
}

void AInfluenceMapGrid::AddUpdateTileTeam2(const uint32 index) {
	for (int i = 0; i != m_index_team2.Num(); ++i) {
		if (m_index_team2[i] >= index) {
			m_index_team2.Insert(index, i);
			return;
		}
	}
	m_index_team2.Add(index);
}