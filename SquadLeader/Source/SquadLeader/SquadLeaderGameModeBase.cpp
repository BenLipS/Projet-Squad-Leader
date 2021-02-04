#include "SquadLeaderGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Characters/Soldiers/SoldierPlayerController.h"
#include "Characters/Soldiers/SoldierPlayerState.h"

ASquadLeaderGameModeBase::ASquadLeaderGameModeBase() {
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("/Game/BluePrints/Characters/BP_SoldierHeavy"));
	static ConstructorHelpers::FClassFinder<ASoldierPlayerController> PlayerControllerObject(TEXT("/Game/BluePrints/Controllers/BP_SoldierPlayerController"));
	static ConstructorHelpers::FClassFinder<ASoldierPlayerState> PlayerStateObject(TEXT("/Game/BluePrints/States/BP_SoldierPlayerState"));

	if (PlayerPawnObject.Class != NULL)
		DefaultPawnClass = PlayerPawnObject.Class;

	if (PlayerControllerObject.Class != NULL)
		PlayerControllerClass = PlayerControllerObject.Class;

	if (PlayerStateObject.Class != NULL)
		PlayerStateClass = PlayerStateObject.Class;
}

void ASquadLeaderGameModeBase::StartPlay() {
	Super::StartPlay();
}