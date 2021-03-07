// Fill out your copyright notice in the Description page of Project Settings.


#include "GrdiSetUp.h"
#include "../../soldiers/soldier.h"
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

		CleanGrid();
		UpdateGrid();
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
				FVector _location = FVector(dist_X, dist_Y, 800.f);
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
			if(IsValidLocation(_location, navSys)) 
				AddGridBase(i, j, true);
			else
				AddGridBase(i, j, false);
			dist_Y += m_GridBaseSize_Y;
		}
		dist_X += m_GridBaseSize_X;
		dist_Y = m_GridBaseSize_Y / 2;
	}
}

bool AGrdiSetUp::IsValidLocation(FVector _location, UNavigationSystemV1* navSys) {
	UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), FVector(11740.f, 7030.f, 20.f), _location);
	return path->IsValid() && (!path->IsPartial());
	
	//We found out that Unreal said a point is good even when the path doens't reach the location
	//So this explain the line "path->PathPoints.Last() - _location).Size() < 50.f"
	//If the last point of the Path is near the location then it's a good location
}

void AGrdiSetUp::AddGridBase(int index_i, int index_j, bool _isvalid) {
	if(_isvalid)
		m_GridBases[index_i * size_array_X + index_j] = 0.f;
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
			if(m_GridBases[i * size_array_X + j] < 100.f)
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

			int controlValue_team = 0;
			float possessTeam = 0;

			for (auto _stat : m_controlArea->TeamData) {
				if (controlValue_team < _stat.Value->controlValue) {
					controlValue_team = _stat.Value->controlValue;
					if (_stat.Key.GetDefaultObject()->TeamName == "Team1")
						possessTeam = static_cast<float>(controlValue_team) / static_cast<float>(m_controlArea->maxControlValue);
					else
						possessTeam = -(static_cast<float>(controlValue_team) / static_cast<float>(m_controlArea->maxControlValue));
				}
			}
			if (possessTeam != 0.f) {
				SetValue(_index_tile_origin, possessTeam);
				SetRadiusValue(_index_tile_origin, possessTeam, 10);
			}
			
		}
	}
}

void AGrdiSetUp::SetValue(int _index, float _value) {
	//cehck that we are in the grid and not on a invalid block
	if (_index >= 0 && _index < size_array_X * size_array_Y && m_GridBases[_index] < 100.f) {
		if (m_GridBases[_index] + _value <= 1.0f && m_GridBases[_index] + _value >= -1.0f)
			m_GridBases[_index] += _value;
		else if (m_GridBases[_index] + _value >= 1.0f)
			m_GridBases[_index] = 1.f;
		else
			m_GridBases[_index] = -1.f;
	}
}

void AGrdiSetUp::SetRadiusValue(int _index, float _value, int _radius) {

	/*
	* i represent the distance between the _index and the other
	* we stop when the distance is equal to the radius of influence
	*/
	for (int i = 0; i != _radius; ++i) {
		//calculate the value
		float value_index = _value/ FMath::Square((FMath::Abs(1.f + (i + 1.f)))) *2.f;

		/*
		* Get the four corner of the influence
		*/
		int index_left_up = _index - (i + 1) + ((i + 1) * size_array_Y);
		int index_left_down = _index - (i + 1) - ((i + 1) * size_array_Y);
		int index_rigth_up = _index + (i + 1) + ((i + 1) * size_array_Y);
		int index_rigth_down = _index + (i + 1) - ((i + 1) * size_array_Y);

		/*
		* for each side we set the value
		*/
		for (int x = index_left_up; x != index_rigth_up; ++x)
			SetValue(x, value_index);
		for (int x = index_left_down; x != index_rigth_down; ++x)
			SetValue(x, value_index);
		//here we go down because we already set the value in index_left_up
		for (int x = index_left_up - size_array_Y; x != index_left_down; x -= size_array_Y)
			SetValue(x, value_index);
		//here we go down because we need to set the index_right_down
		for (int x = index_rigth_up; x != index_rigth_down - size_array_Y; x -= size_array_Y)
			SetValue(x, value_index);
	}
}

float AGrdiSetUp::GetValue(FVector _location) {
	return m_GridBases[FindTile(_location)];
}