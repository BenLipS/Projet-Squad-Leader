// Fill out your copyright notice in the Description page of Project Settings.


#include "GrdiSetUp.h"
#include "../../Soldiers/Soldier.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGrdiSetUp::AGrdiSetUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGrdiSetUp::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority) {
		CreateGrid();
		m_Gamemode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
	}
	
}

// Called every frame
void AGrdiSetUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority) {
		m_nbr_tick++;
		switch (m_nbr_tick) {
		case 1:
			CleanGrid();
			break;
		case 2:
			UpdateGrid();
			m_nbr_tick = 0;
			break;
		default:
			break;
		}
		DrawGrid();
	}
}

void AGrdiSetUp::DrawGrid() {
	int dist_X = m_GridBaseSize_X / 2;
	int dist_Y = m_GridBaseSize_Y / 2;
	int test = 0;
	for (int i = 0; i != size_array_X; ++i) {
		for (int j = 0; j != size_array_Y; ++j) {
			if (m_GridBases[i * size_array_X + j] <= 1.f) {
				FVector _location = FVector(dist_X, dist_Y, 30.f);
				if (m_GridBases[i * size_array_X + j] > 0.f)
					DrawDebugSolidBox(GetWorld(), _location, FVector(95.f, 95.f, 10.f), FColor(0,0, m_GridBases[i * size_array_X + j] * 255));
				else if (m_GridBases[i * size_array_X + j] < 0.f)
					DrawDebugSolidBox(GetWorld(), _location, FVector(95.f, 95.f, 10.f), FColor(m_GridBases[i * size_array_X + j] * 255, 0, 0));
			}
			
			dist_Y += m_GridBaseSize_Y;
		}
		dist_X += m_GridBaseSize_X;
		dist_Y = m_GridBaseSize_Y / 2;
	}
}

void AGrdiSetUp::CreateGrid() {
	size_array_X = m_GridSize_X / m_GridBaseSize_X;
	size_array_Y = m_GridSize_Y / m_GridBaseSize_Y;
	size_array = size_array_X * size_array_Y;
	m_GridBases.Init(NULL, size_array);

	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	int dist_X = m_GridBaseSize_X / 2;
	int dist_Y = m_GridBaseSize_Y / 2;
	int test = 0;
	for (int i = 0; i != size_array_X; ++i) {
		for (int j = 0; j != size_array_Y; ++j) {
			FVector _location = FVector(dist_X, dist_Y, 30.f);
			IsValidLocation(_location, navSys)? AddGridBase(i, j, _location) : AddGridBase(i, j, FVector());
			dist_Y += m_GridBaseSize_Y;
		}
		dist_X += m_GridBaseSize_X;
		dist_Y = m_GridBaseSize_Y / 2;
	}
}

bool AGrdiSetUp::IsValidLocation(FVector _location, UNavigationSystemV1* navSys) {
	UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), FVector(0.f, 0.f, 0.f), _location);
	return path->IsValid() && (!path->IsPartial());
	
	//We found out that Unreal said a point is good even when the path doens't reach the location
	//So this explain the line "path->PathPoints.Last() - _location).Size() < 50.f"
	//If the last point of the Path is near the location then it's a good location
}

void AGrdiSetUp::AddGridBase(int index_i, int index_j, FVector _location) {
	if (_location.Size() > 0.f) {
		m_GridBases[index_i * size_array_X + index_j] = 0.f;
	}
	else
		m_GridBases[index_i * size_array_X + index_j] = 100.f;
}

int AGrdiSetUp::FindTile(FVector _location) {

	int index_i = _location.X / m_GridBaseSize_X;
	int index_j = _location.Y / m_GridBaseSize_Y;

	if (index_i < 0.f)
		index_i = 0.f;
	if (index_j < 0.f)
		index_j = 0.f;

	if (index_i < size_array_X && index_j < size_array_Y && m_GridBases[index_i * size_array_X + index_j] <= 1.0f)
		return index_i * size_array_X + index_j;
	return -1;
}

void AGrdiSetUp::CleanGrid() {
	for (int i = 0; i != size_array_X; ++i) {
		for (int j = 0; j != size_array_Y; ++j) {
			m_GridBases[i * size_array_X + j] = 0.f;
		}
	}
}

void AGrdiSetUp::UpdateGrid(){
	UpdateGridSoldier();
	UpdateGridControlArea();
}

void AGrdiSetUp::UpdateGridSoldier() {
	int _team_value = 1;
	for (TSubclassOf<ASoldierTeam> _team : m_Gamemode->SoldierTeamCollection) {
		
		ASoldierTeam* _teamclass = Cast<ASoldierTeam>(_team->GetDefaultObject());
		
		for (ASoldier* _soldier : _teamclass->soldierList) {
			
			int _index_tile_origin = FindTile(_soldier->GetLocation());
			float _value_im = _soldier->InfluenceWeight * _team_value;

			SetValue(_index_tile_origin, _value_im);
			SetRadiusValue(_index_tile_origin, _value_im, _soldier->InfluenceRadius);
		}

		_team_value = -1;
	}
}

void AGrdiSetUp::UpdateGridControlArea() {

}

void AGrdiSetUp::SetValue(int _index, float _value) {
	if (_index >= 0) {
		if (m_GridBases[_index] + _value <= 1.0f)
			m_GridBases[_index] += _value;
		else
			m_GridBases[_index] = 1.f;
	}
		
}

void AGrdiSetUp::SetRadiusValue(int _index, float _value, int _radius) {
	float _value_im_side = 0.f;

	int _index_tile_left = _index;
	int _index_tile_right = _index;
	int _index_tile_up = _index;
	int _index_tile_down = _index;

	for (int i = 0; i != _radius; ++i) {
		_index_tile_up -= size_array_Y;
		_index_tile_down += size_array_Y;
		_index_tile_left -= 1;
		_index_tile_right += 1;
		_value_im_side = _value / FMath::Square(1.f + (i + 1));

		SetValue(_index_tile_up, _value_im_side);
		SetValue(_index_tile_down, _value_im_side);
		SetValue(_index_tile_left, _value_im_side);
		SetValue(_index_tile_right, _value_im_side);
	}
}