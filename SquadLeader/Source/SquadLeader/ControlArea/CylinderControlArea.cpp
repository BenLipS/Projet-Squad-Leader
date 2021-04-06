#include "CylinderControlArea.h"

// Sets default values
ACylinderControlArea::ACylinderControlArea()
{
	initCollideElement();

	PrimaryActorTick.bCanEverTick = false;
}

// used when initialising the control area
void ACylinderControlArea::initCollideElement() {
	CylinderCollide = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleContactComponent"));
	CylinderCollide->SetCapsuleRadius(30, false);
	CylinderCollide->SetCapsuleHalfHeight(40, false);
	RootComponent = CylinderCollide;
}

// Called when the game starts or when spawned
void ACylinderControlArea::BeginPlay()
{
	Super::BeginPlay();
}