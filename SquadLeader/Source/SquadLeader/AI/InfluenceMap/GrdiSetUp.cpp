// Fill out your copyright notice in the Description page of Project Settings.


#include "GrdiSetUp.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

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
	for (int i = 0; i != size_array_X; ++i)
		for (int j = 0; j != size_array_Y; ++j)
			if (m_GridBases[i * size_array_X + j] != NULL && m_GridBases[i * size_array_X + j]->IsValid())
				m_GridBases[i * size_array_X + j]->DrawBase(GetWorld());
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
			UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), FVector(0.f, 0.f, 0.f), _location);
			if (path->IsValid() && (path->PathPoints.Last()-_location).Size()<50.f) {
				m_GridBases[i * size_array_X + j] = NewObject<UGridBase>();
				m_GridBases[i * size_array_X + j]->SetIsValid(true);
				m_GridBases[i * size_array_X + j]->SetLocation(_location);
			}
			else {
				m_GridBases[i * size_array_X + j] = NULL;
			}
			dist_Y += m_GridBaseSize_Y;
		}

		dist_X += m_GridBaseSize_X;
		dist_Y = m_GridBaseSize_Y / 2;
	}
}