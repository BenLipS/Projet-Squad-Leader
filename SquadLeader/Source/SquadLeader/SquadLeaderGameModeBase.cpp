#include "SquadLeaderGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Soldiers/Players/SoldierPlayerController.h"
#include "Soldiers/Players/SoldierPlayerState.h"

ASquadLeaderGameModeBase::ASquadLeaderGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("/Game/BluePrints/Players/BP_SoldierPlayerSupport"));
	static ConstructorHelpers::FClassFinder<ASoldierPlayerController> PlayerControllerObject(TEXT("/Game/BluePrints/Players/BP_SoldierPlayerController"));
	static ConstructorHelpers::FClassFinder<ASoldierPlayerState> PlayerStateObject(TEXT("/Game/BluePrints/Players/BP_SoldierPlayerState"));

	if (PlayerPawnObject.Class != NULL)
		DefaultPawnClass = PlayerPawnObject.Class;

	if (PlayerControllerObject.Class != NULL)
		PlayerControllerClass = PlayerControllerObject.Class;

	if (PlayerStateObject.Class != NULL)
		PlayerStateClass = PlayerStateObject.Class;
}

void ASquadLeaderGameModeBase::StartPlay() {
	// init collections
	controlAreaCollection = {};

	// todo : create teams as class


	Super::StartPlay();
}

void ASquadLeaderGameModeBase::RespawnSoldier(AController* _Controller)
{
	if (ASoldier* soldier = Cast<ASoldier>(_Controller->GetPawn()); soldier)
	{
		// TODO: improve respawn
		soldier->SetActorTransform(FTransform(FQuat(), FVector(0.f, 0.f, 400.f)));
		soldier->Respawn();
	}
}