// Fill out your copyright notice in the Description page of Project Settings.


#include "GrdiSetUp.h"

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
	CreateGrid();
}

// Called every frame
void AGrdiSetUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DrawGrid();
}

void AGrdiSetUp::DrawGrid() {
	for(UGridBase* _gb : m_GridBases)
		if (_gb != NULL && _gb->IsValid())
			_gb->DrawBase(GetWorld());
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
	return path->IsValid() && (path->PathPoints.Last() - _location).Size() < 50.f;
	
	//We found out that Unreal said a point is good even when the path doens't reach the location
	//So this explain the line "path->PathPoints.Last() - _location).Size() < 50.f"
	//If the last point of the Path is near the location then it's a good location
}

void AGrdiSetUp::AddGridBase(int index_i, int index_j, FVector _location) {
	if (_location.Size() > 0.f) {
		m_GridBases[index_i * size_array_X + index_j] = NewObject<UGridBase>();
		m_GridBases[index_i * size_array_X + index_j]->SetIsValid(true);
		m_GridBases[index_i * size_array_X + index_j]->SetLocation(_location);
	}
	else
		m_GridBases[index_i * size_array_X + index_j] = NULL;
}