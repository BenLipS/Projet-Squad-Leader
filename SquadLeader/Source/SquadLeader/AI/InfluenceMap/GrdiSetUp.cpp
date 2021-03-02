// Fill out your copyright notice in the Description page of Project Settings.


#include "GrdiSetUp.h"
#include "../../Soldiers/Soldier.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../ControlArea/ControlAreaManager.h"
#include "../../ControlArea/ControlArea.h"
#include "../../ControlArea/ControlAreaTeamStat.h"

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
					DrawDebugSolidBox(GetWorld(), _location, FVector(95.f, 95.f, 10.f), FColor(FMath::Abs(m_GridBases[i * size_array_X + j]) * 255, 0, 0));
				/*else
					DrawDebugSolidBox(GetWorld(), _location, FVector(95.f, 95.f, 10.f), FColor(0, 0, 0));*/
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
	AControlAreaManager* m_controlAreaManager = Cast<AControlAreaManager>(m_Gamemode->ControlAreaManager->GetDefaultObject());
	if (m_controlAreaManager) {
		for (AControlArea* m_controlArea : m_controlAreaManager->GetControlArea()) {
			int _index_tile_origin = FindTile(m_controlArea->GetActorLocation());
			float _value_im = 0.f;
			/*m_controlArea*/
			int controlValue_team = 0;
			float possessTeam = 0;

			for (auto _stat : m_controlArea->TeamData) {
				if (controlValue_team < _stat.Value->controlValue) {
					controlValue_team = _stat.Value->controlValue;
					if (_stat.Key.GetDefaultObject()->TeamName == "Team1")
						possessTeam = 1.0f;
					else
						possessTeam = -1.0f;
				}
			}

			SetValue(_index_tile_origin, possessTeam);
			SetRadiusValue(_index_tile_origin, possessTeam, 10);
		}
	}
}

void AGrdiSetUp::SetValue(int _index, float _value) {
	if (_index >= 0 && _index < size_array_X * size_array_Y) {
		if (m_GridBases[_index] + _value <= 1.0f)
			m_GridBases[_index] += _value;
		else
			m_GridBases[_index] = 1.f;
	}
		
}

void AGrdiSetUp::SetRadiusValue(int _index, float _value, int _radius) {
	float _value_im_side = _value;
	int _index_tile_left_up = _index - _radius + (_radius * size_array_Y);
	int _index_tile_left_down = _index - _radius - (_radius * size_array_Y);

	for (int _index_y = _index_tile_left_up; _index_y != _index_tile_left_down; _index_y -= size_array_Y) {
		for (int _index_x = _index_y; _index_x != _index_y + ( 2*_radius ); ++_index_x) {
			SetValue(_index_x, _value_im_side);
		}
	}
}