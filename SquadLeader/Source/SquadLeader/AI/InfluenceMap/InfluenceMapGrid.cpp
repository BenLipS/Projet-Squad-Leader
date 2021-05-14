// Fill out your copyright notice in the Description page of Project Settings.


#include "InfluenceMapGrid.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "../../GameState/SquadLeaderGameState.h"
#include "../../Soldiers/Soldier.h"
#include "Templates/Function.h"
#include "EngineUtils.h"


AInfluenceMapGrid::AInfluenceMapGrid() {
	//Set this AInfo to be call every Tick. 
	PrimaryActorTick.bCanEverTick = true;
}

void AInfluenceMapGrid::BeginPlay() {
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority) {
		InitGrid();
		FindNeighboor();
	}
}

void AInfluenceMapGrid::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if (GetLocalRole() == ROLE_Authority) {
		DrawGrid();
	}
}

void AInfluenceMapGrid::InitGrid() noexcept {
	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	for (int y = m_tile_height / 2.f; y <= m_grid_height; y += m_tile_height) {
		for (int x = m_tile_width / 2.f; x <= m_grid_width; x += m_tile_width) {
			FTileBase Tile;
			auto Location = FVector(x, y, m_startLocation.Z);
			if (IsValid(Location)) {
				Location.Z += m_height;
				Tile.Location = Location;
				Tile.State = TileState::Free;
				Tile.InfluenceTeam.Add(1, FInfluenceTeamData());
				Tile.InfluenceTeam.Add(2, FInfluenceTeamData());
				Grid.Add(Tile);
			}
		}
	}
	nbr_tile = Grid.Num();
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
	const float Size = m_tile_width / 2.f - 5.f;
	if (m_DrawCharacterInfluence) {
		for (const FTileBase& Tile : Grid) {
			const float ValueTeam1 = Tile.InfluenceTeam[1].InfluenceValue;
			const float ValueTeam2 = Tile.InfluenceTeam[2].InfluenceValue;
			
			if (Tile.State == TileState::Block)
				DrawDebugSolidBox(GetWorld(), Tile.Location, FVector(Size, Size, 10.f), FColor::Silver);
			else if (ValueTeam1 > 0.0f && ValueTeam2 > 0.0f)
				DrawDebugSolidBox(GetWorld(), Tile.Location, FVector(Size, Size, 10.f), FColor(255 * ValueTeam2, 0, 255 * ValueTeam1));
			else {
				if (ValueTeam1 > 0.0f)
					DrawDebugSolidBox(GetWorld(), Tile.Location, FVector(Size, Size, 10.f), FColor(0, 0, 255 * ValueTeam1));
				else if (ValueTeam2 > 0.0f)
					DrawDebugSolidBox(GetWorld(), Tile.Location, FVector(Size, Size, 10.f), FColor(255 * ValueTeam2, 0, 0));
			}
		}
	}
	if (m_DrawAllGrid) {
		for (const FTileBase& Tile : Grid)
			DrawDebugSolidBox(GetWorld(), Tile.Location, FVector(Size, Size, 10.f), FColor::Green);

	}
}

void AInfluenceMapGrid::UpdateGrid() noexcept {

	//for (int32 index = m_index_team1.Num() - 1; index >= 0; --index) {
	//	const uint32 index_tile = m_index_team1[index];
	//	if (!(m_influencemap[index_tile].Types.Contains(Type::ControlArea))) {
	//		m_influencemap[index_tile].Reset();
	//	}
	//	else {
	//		m_influencemap[index_tile].m_value = ControlAreaInfluence;
	//	}
	//}

	//for (int32 index = m_index_team2.Num() - 1; index >= 0; --index) {
	//	const uint32 index_tile = m_index_team2[index];
	//	if (!(m_influencemap[index_tile].Types.Contains(Type::ControlArea))) {
	//		m_influencemap[index_tile].Reset();
	//	}
	//	else {
	//		m_influencemap[index_tile].m_value = ControlAreaInfluence;
	//	}
	//}

	//auto functor = [&](const uint32 index) { return !m_influencemap[index].in_update; };
	//m_index_team1.RemoveAll(functor);
	//m_index_team2.RemoveAll(functor);
}

bool AInfluenceMapGrid::FindTileIndex(const FVector Location, uint32& Index) const noexcept {
	int begin = 0;
	int end = Grid.Num() - 1;
	int middle = begin;
	bool find = false;

	while (!find && begin <= end) {
		middle = (begin + end) / 2;
		const FTileBase Tile = Grid[middle];
		if (IsOnTile(Location, Tile.Location))
			find = true;
		else if (Location.Y >= Tile.Location.Y - (m_tile_height / 2.0f) && Location.Y <= Tile.Location.Y + (m_tile_height / 2.f))
			if (Location.X > Tile.Location.X)
				begin = middle + 1;
			else
				end = middle - 1;
		else
			if (Location.Y > Tile.Location.Y - (m_tile_height / 2.0f))
				begin = middle + 1;
			else
				end = middle - 1;
	}
	if (find) {
		Index = middle;
		return true;
	}
	return false;
}

bool AInfluenceMapGrid::FindIndexModifyinTeam1(const FVector2D Location, uint32& Index) {
	if (m_index_team1.Num() > 0) {
		int begin = 0;
		int end = m_index_team1.Num() - 1;
		int middle = begin;
		bool find = false;

		while (!find && begin <= end) {
			middle = (begin + end) / 2;
			const FTileBase Tile = Grid[m_index_team1[middle]];
			if (IsOnTileUpdate(Location, Tile.Location))
				find = true;
			else if (Location.Y >= Tile.Location.Y - (m_tile_height / 2.0f) && Location.Y <= Tile.Location.Y + (m_tile_height / 2.f))
				if (Location.X > Tile.Location.X)
					begin = middle + 1;
				else
					end = middle - 1;
			else
				if (Location.Y > Tile.Location.Y - (m_tile_height / 2.0f))
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
			const FTileBase Tile = Grid[m_index_team2[middle]];
			if (IsOnTileUpdate(Location, Tile.Location))
				find = true;
			else if (Location.Y >= Tile.Location.Y - (m_tile_height / 2.0f) && Location.Y <= Tile.Location.Y + (m_tile_height / 2.f))
				if (Location.X > Tile.Location.X)
					begin = middle + 1;
				else
					end = middle - 1;
			else
				if (Location.Y > Tile.Location.Y - (m_tile_height / 2.0f))
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
	const float width = m_tile_width / 2.f;
	const float height = m_tile_height / 2.f;
	const bool on_axis_x = _location.X <= tile_location.X + width && _location.X >= tile_location.X - width;
	const bool on_axis_y = _location.Y <= tile_location.Y + height && _location.Y >= tile_location.Y - height;

	return on_axis_x && on_axis_y;
}

bool AInfluenceMapGrid::IsOnTileUpdate(const FVector2D Location, const FVector TileLocation) const noexcept {
	const float width = m_tile_width / 2.f;
	const float height = m_tile_height / 2.f;

	const bool AxisX = (Location.X <= TileLocation.X + width) && (Location.X >= TileLocation.X - width);
	const bool AxisY = (Location.Y <= TileLocation.Y + height) && (Location.Y >= TileLocation.Y - height);

	return AxisX && AxisY;
}

void AInfluenceMapGrid::Neighboors(int index) noexcept {
	uint32 index_up = -1;
	uint32 index_down = -1;

	const int index_left = index - 1;
	const int index_right = index + 1;

	if (FindTileIndex(FVector(Grid[index].Location.X, Grid[index].Location.Y + m_tile_height, Grid[index].Location.Z), index_up))
		IsNeighboor(index, index_up);
	if (index_right < nbr_tile)
		IsNeighboor(index, index_right);

	if (FindTileIndex(FVector(Grid[index].Location.X, Grid[index].Location.Y - m_tile_width, Grid[index].Location.Z), index_down))
		IsNeighboor(index, index_down);
	if (index_left >= 0)
		IsNeighboor(index, index_left);
}

void AInfluenceMapGrid::IsNeighboor(int index_1, int index_2) noexcept {
	int distance = FVector::Dist(Grid[index_1].Location, Grid[index_2].Location);
	if (distance <= m_tile_width) {
		Grid[index_1].Neighboors.m_neighboor.Add(index_2);
	}
}

void AInfluenceMapGrid::FindNeighboor()noexcept {
	for (int i = 0; i != nbr_tile; ++i)
		Neighboors(i);
}

void AInfluenceMapGrid::CalculateSoldierInfluence(int index, int start_index, int source_index, int distance, uint32 actorID, uint32 Team, uint16& ActorDataIndex) noexcept {
	if (distance > CharacterAreaInfluence)
		return;

	for (int neighboor : Grid[index].Neighboors.m_neighboor) {
		if (neighboor != start_index && Grid[neighboor].State == TileState::Free) {
			UpdateTile(neighboor, CharacterInfluenceValue, Team, Type::Soldier, actorID, ActorDataIndex);
			CalculateSoldierInfluence(neighboor, index, source_index, distance + 1, actorID, Team, ActorDataIndex);
		}
	}
}

void AInfluenceMapGrid::CalculateControlAreaInfluence(int index, int start_index, int source_index, int distance, uint32 actorID, uint32 Team, uint16& ActorDataIndex) noexcept {
	if (distance > ControlAreaAreaInfluence)
		return;

	for (int neighboor : Grid[index].Neighboors.m_neighboor) {
		if (neighboor != start_index && Grid[neighboor].State == TileState::Free) {
			UpdateTile(neighboor, ControlAreaInfluenceValue, Team, Type::ControlArea, actorID, ActorDataIndex);
			CalculateControlAreaInfluence(neighboor, index, source_index, distance + 1, actorID, Team, ActorDataIndex);
		}
	}
}

void AInfluenceMapGrid::CalculateProjectileInfluence(int index, int start_index, int source_index, int distance, uint32 actorID, uint32 Team, uint16& ActorDataIndex) noexcept {

	if (distance > ProjectileAreaInfluence)
		return;

	for (int neighboor : Grid[index].Neighboors.m_neighboor) {
		if (neighboor != start_index && Grid[neighboor].State == TileState::Free) {
			UpdateTile(neighboor, ProjectileInfluenceValue, Team, Type::Projectile, actorID, ActorDataIndex);
			CalculateProjectileInfluence(neighboor, index, source_index, distance + 1, actorID, Team, ActorDataIndex);
		}
	}
}

void AInfluenceMapGrid::ReceivedMessage(FGridPackage _message) {
	uint32 Index = -1;

	if (FindTileIndex(_message.m_location_on_map, Index)) {

		uint16 IndexActor = 0;
		if (!ActorAlreadyExist(_message.ActorID, IndexActor)) {
			IndexActor = ActorsData.Num();
			FActorData actorData;
			actorData.ActorID = _message.ActorID;
			ActorsData.Add(actorData);

			switch (_message.m_type) {
			case Type::Soldier:
				SoldierInfluence(_message, Index, IndexActor);
				break;
			case Type::ControlArea:
				ControlAreaInfluence(_message, Index, IndexActor);
				break;
			case Type::Projectile:
				ProjectileInfluence(_message, Index, IndexActor);
				break;
			default:
				break;
			}

		}
		else {
			switch (_message.m_type) {
			case Type::Soldier:
				UpdateSoldier(IndexActor, _message.team_value, _message.ActorID);
				UpdateTile(Index, CharacterInfluenceValue, _message.team_value, _message.m_type, _message.ActorID, IndexActor);
				CalculateSoldierInfluence(Index, Index, Index, 1, _message.ActorID, _message.team_value, IndexActor);
				break;
			case Type::ControlArea:
				UpdateControlArea(IndexActor, _message.team_value);
				break;
			case Type::Projectile:
				DeleteInfluence(IndexActor, _message.team_value);
				break;
			default:
				break;
			}
		}
	}
}

void AInfluenceMapGrid::ReceivedMessage(FGridPackageObstacle Message) {
	uint32 Index = -1;

	if (FindTileIndex(Message.m_location_on_map, Index)) {

		uint16 IndexActor = 0;
		if (!ActorAlreadyExist(Message.ActorID, IndexActor)) {
			IndexActor = ActorsData.Num();
			FActorData actorData;
			actorData.ActorID = Message.ActorID;
			actorData.AddIndexs(IndexActor);
			ActorsData.Add(actorData);
			ObstacleInfluence(Message, Index, IndexActor);
		}
	}
}

void AInfluenceMapGrid::TimeFunction() {
	const double startTime = FPlatformTime::Seconds();

	// code de la fonction

	const double endTime = FPlatformTime::Seconds();
	const double elapsedTime = endTime - startTime;
}

void AInfluenceMapGrid::UpdateTile(int index, float value, int team, Type type, uint32 actorID, uint16& ActorDataIndex) noexcept {

	auto bool2 = Grid[index].InfluenceTeam[team].ActorsID.Contains(actorID);

	if (!bool2) {

		if (type == Type::ControlArea) {
			Grid[index].InfluenceTeam[team].InfluenceValue = ControlAreaInfluenceValue + (0.1f * Grid[index].InfluenceTeam[team].ActorsID.Num());
			Grid[index].InfluenceTeam[team].Types.Add(type);
		}
		else {
			const float ValueInfluenceTile = Grid[index].InfluenceTeam[team].InfluenceValue;


			if (ValueInfluenceTile >= 0.3f)
				if(ValueInfluenceTile + 0.1f > 1.0f)
					Grid[index].InfluenceTeam[team].InfluenceValue = 1.f;
				else
					Grid[index].InfluenceTeam[team].InfluenceValue += 0.1f;
			else
				Grid[index].InfluenceTeam[team].InfluenceValue = 0.3f;

			Grid[index].InfluenceTeam[team].Types.Add(type);
		}

		if (team == 1)
			AddUpdateTileTeam1(index);
		else
			AddUpdateTileTeam2(index);


		Grid[index].InfluenceTeam[team].ActorsID.Add(actorID);

		ActorsData[ActorDataIndex].AddIndexs(index);
	}
}

float AInfluenceMapGrid::GetValue(const FVector2D Location, const uint8 Team) {
	uint32 index = 0;
	float Cost = 0.0f;

	switch (Team) {
	case 1:
		if (FindIndexModifyinTeam1(Location, index)) {
			const float Value = Grid[index].InfluenceTeam[2].InfluenceValue;
			if (Value > 0.0f)
				Cost = Value;
		}
		break;
	case 2:
		if (FindIndexModifyinTeam2(Location, index)) {
			const float Value = Grid[index].InfluenceTeam[1].InfluenceValue;
			if (Value > 0.0f)
				Cost = Value;
		}
		break;

	default:
		break;
	}
	return Cost * 100.f;
}

void AInfluenceMapGrid::AddUpdateTileTeam1(const uint32 index) {
	for (int i = 0; i != m_index_team1.Num(); ++i) {
		if (m_index_team1[i] > index) {
			m_index_team1.Insert(index, i);
			return;
		}
		if (m_index_team1[i] == index)
			return;
	}
	m_index_team1.Add(index);
}

void AInfluenceMapGrid::AddUpdateTileTeam2(const uint32 index) {
	for (int i = 0; i != m_index_team2.Num(); ++i) {
		if (m_index_team2[i] > index) {
			m_index_team2.Insert(index, i);
			return;
		}
		if (m_index_team2[i] == index)
			return;
	}
	m_index_team2.Add(index);
}

bool AInfluenceMapGrid::ActorAlreadyExist(const uint32 ActorID, uint16& Index) const {
	auto ExistActor = [](const uint32 actorID, const FActorData& actorData) { return actorID == actorData.ActorID; };
	uint16 index = 0;
	for (FActorData actorData : ActorsData) {
		if (ExistActor(ActorID, actorData)) {
			Index = index;
			return true;
		}
		index++;
	}
	return false;
}

void AInfluenceMapGrid::DeleteInfluence(const uint16 IndexActor, const uint8 Team) noexcept {
	for (uint32 index : ActorsData[IndexActor].IndexInfluence) {
		//Pour le moment c'est seulement les Obstacles
		/*if (Team == 1)
			m_index_team1.Remove(index);
		else
			m_index_team2.Remove(index);*/
		m_index_team1.Remove(index);
		m_index_team2.Remove(index);
	}
	ActorsData[IndexActor].IndexInfluence.Empty();
	//To-Do : besoin de créer l'operateur == pour des FActorData
	//ActorsData.Remove(ActorsData[IndexActor]);
}

void AInfluenceMapGrid::UpdateControlArea(const uint16 IndexControlArea, const uint8 Team) noexcept {

	switch (Team) {
	case 1:
		for (uint32 index : ActorsData[IndexControlArea].IndexInfluence) {
			m_index_team2.Remove(index);
			m_index_team1.Add(index);
			Grid[index].InfluenceTeam[2].InfluenceValue -= ControlAreaInfluenceValue;
			Grid[index].InfluenceTeam[1].InfluenceValue += ControlAreaInfluenceValue;
		}
		break;
	case 2:
		for (uint32 index : ActorsData[IndexControlArea].IndexInfluence) {
			m_index_team1.Remove(index);
			m_index_team2.Add(index);
			Grid[index].InfluenceTeam[1].InfluenceValue -= ControlAreaInfluenceValue;
			Grid[index].InfluenceTeam[2].InfluenceValue += ControlAreaInfluenceValue;
		}
		break;
	default:
		break;
	}

}

void AInfluenceMapGrid::UpdateSoldier(const uint16 IndexSoldier, const uint8 Team, const uint32 SoldierID) noexcept {
	if (ActorsData[IndexSoldier].IndexInfluence.Num() > 0) {
		for (uint32 index : ActorsData[IndexSoldier].IndexInfluence) {
			Grid[index].InfluenceTeam[Team].ActorsID.Remove(SoldierID);

			if (Grid[index].InfluenceTeam[Team].ActorsID.Num() <= 0) {
				Grid[index].InfluenceTeam[Team].InfluenceValue = 0.0f;
				switch (Team) {
				case 1:
					m_index_team1.Remove(index);
					break;
				case 2:
					m_index_team2.Remove(index);
					break;
				default:
					break;
				}
			}
			else {
				Grid[index].InfluenceTeam[Team].InfluenceValue -= 0.1f;
				if (Grid[index].InfluenceTeam[Team].Types.Contains(Type::ControlArea)) {
					if (Grid[index].InfluenceTeam[Team].InfluenceValue < ControlAreaInfluenceValue)
						Grid[index].InfluenceTeam[Team].InfluenceValue = ControlAreaInfluenceValue;
				}
				else if(Grid[index].InfluenceTeam[Team].Types.Contains(Type::Soldier)) {
					if (Grid[index].InfluenceTeam[Team].InfluenceValue < CharacterInfluenceValue)
						Grid[index].InfluenceTeam[Team].InfluenceValue = CharacterInfluenceValue;
				}
			}
		}
		ActorsData[IndexSoldier].IndexInfluence.Empty();
	}
}

void AInfluenceMapGrid::SoldierInfluence(FGridPackage Message, uint32 IndexTile, uint16 IndexActor) {
	UpdateTile(IndexTile, CharacterInfluenceValue, Message.team_value, Message.m_type, Message.ActorID, IndexActor);
	CalculateSoldierInfluence(IndexTile, IndexTile, IndexTile, 1, Message.ActorID, Message.team_value, IndexActor);
}
void AInfluenceMapGrid::ControlAreaInfluence(FGridPackage Message, uint32 IndexTile, uint16 IndexActor) {
	UpdateTile(IndexTile, ControlAreaInfluenceValue, Message.team_value, Message.m_type, Message.ActorID, IndexActor);
	CalculateControlAreaInfluence(IndexTile, IndexTile, IndexTile, 1, Message.ActorID, Message.team_value, IndexActor);
}
void AInfluenceMapGrid::ProjectileInfluence(FGridPackage Message, uint32 IndexTile, uint16 IndexActor) {
	UpdateTile(IndexTile, ProjectileInfluenceValue, Message.team_value, Message.m_type, Message.ActorID, IndexActor);
	CalculateProjectileInfluence(IndexTile, IndexTile, IndexTile, 1, Message.ActorID, Message.team_value, IndexActor);
}
void AInfluenceMapGrid::ObstacleInfluence(FGridPackageObstacle Message, uint32 IndexTile, uint16 IndexActor) {
	//GEngine->AddOnScreenDebugMessage(30, 1.f, FColor::Orange, TEXT("Received Message from Obstacle"));
	Grid[IndexTile].State = TileState::Block;
	ActorsData[IndexActor].AddIndexs(IndexTile);

	uint32 LeftIndex = -1;
	uint32 RightIndex = -1;

	for (FVector Location : Message.Locations) {
		if (FindTileIndex(Location, LeftIndex)) {
			Grid[LeftIndex].State = TileState::Block;
			ActorsData[IndexActor].AddIndexs(LeftIndex);
		}
	}

	
}

void AInfluenceMapGrid::EraseObstacleInfluence(FGridPackageObstacle Message) {
	//GEngine->AddOnScreenDebugMessage(30, 1.f, FColor::Orange, TEXT("Received Message from Obstacle, need to be destroy"));
	uint16 IndexActor = -1;
	if (ActorAlreadyExist(Message.ActorID, IndexActor)) {
		for (uint32 Index : ActorsData[IndexActor].IndexInfluence)
			Grid[Index].State = TileState::Free;
	}
}

double AInfluenceMapGrid::GetInfluenceAverage(const uint32 ActorID, const uint8 Team) {
	//GEngine->AddOnScreenDebugMessage(20, 5.f, FColor::Blue, TEXT("Début du calcule de la moyenne...."));
	for (FActorData& ActorData : ActorsData) {
		if (ActorData.ActorID == ActorID) {
			float Value = 0.f;
			for (uint32 Index : ActorData.IndexInfluence) {
				Value += Grid[Index].InfluenceTeam.Find(Team)->InfluenceValue;
			}
			float Average = Value / ActorData.IndexInfluence.Num();
			return Average;
		}
	}
	return 0.0;
}