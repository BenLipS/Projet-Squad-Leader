// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBase.h"

// Sets default values
AGridBase::AGridBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger box"));
    TriggerBox->SetBoxExtent(FVector(64.f, 64.f, 1.f), false);
    TriggerBox->SetCollisionProfileName(TEXT("OverlapAll"));
    TriggerBox->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AGridBase::BeginPlay()
{
	Super::BeginPlay();
	m_value = 0.0f;
 
}

// Called every frame
void AGridBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    TArray<AActor*> _array_wall; 
    GetOverlappingActors(_array_wall, AWall::StaticClass());
    if (_array_wall.Num() > 0)
        Destroy();
}

void AGridBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
    if (OtherActor && (OtherActor != this))
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Rentré dans la collision box d'une GridBase"));
    }
}


void AGridBase::NotifyActorEndOverlap(AActor* OtherActor) {
    if (OtherActor && (OtherActor != this))
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("This is the end....."));
    }
}