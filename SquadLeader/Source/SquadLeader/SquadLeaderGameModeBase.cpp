#include "SquadLeaderGameModeBase.h"
#include "UObject/ConstructorHelpers.h"

ASquadLeaderGameModeBase::ASquadLeaderGameModeBase() {
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("/Game/BluePrints/Characters/BP_SoldierSupport"));

	if (PlayerPawnObject.Class != NULL)
		DefaultPawnClass = PlayerPawnObject.Class;
}

void ASquadLeaderGameModeBase::StartPlay() {
	Super::StartPlay();
}