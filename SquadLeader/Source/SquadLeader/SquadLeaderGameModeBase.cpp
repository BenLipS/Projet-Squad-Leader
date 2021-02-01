#include "SquadLeaderGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "SoldierPlayerController.h"

ASquadLeaderGameModeBase::ASquadLeaderGameModeBase() {
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("/Game/BluePrints/Characters/BP_SoldierSupport"));
	static ConstructorHelpers::FClassFinder<ASoldierPlayerController> PlayerControllerObject(TEXT("/Game/BluePrints/Controllers/BP_SoldierPlayerController"));

	PlayerControllerClass = PlayerControllerObject.Class;

	if (PlayerPawnObject.Class != NULL)
		DefaultPawnClass = PlayerPawnObject.Class;
}

void ASquadLeaderGameModeBase::StartPlay() {
	Super::StartPlay();
}