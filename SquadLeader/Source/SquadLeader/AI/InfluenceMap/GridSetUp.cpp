// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSetUp.h"

// Sets default values
AGridSetUp::AGridSetUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGridSetUp::BeginPlay()
{
	Super::BeginPlay();
	int value_1 = Grid_X * Grid_Size / 2;
	int value_2 = Grid_Y * Grid_Size / 2;
	for (int x = 0; x != Grid_X; ++x) {
		for (int y = 0; y != Grid_Y; ++y) {
			float value_x = (x * Grid_Size - value_1);
			float value_y = (y * Grid_Size - value_2);
			FVector _position = FVector(value_x, value_y, 0.f);
			FVector _final_position = _position + GetActorLocation();
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AGridBase* _gridbase = GetWorld()->SpawnActor<AGridBase>(m_BP_AGridBase, _final_position, FRotator(0.f,0.f,0.f), SpawnInfo);
			m_GridBases.Add(_gridbase);
		}
	}
}

// Called every frame
void AGridSetUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

