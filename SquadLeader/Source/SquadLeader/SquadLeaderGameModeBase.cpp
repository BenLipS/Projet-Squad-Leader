#include "SquadLeaderGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Soldiers/Players/SoldierPlayerController.h"
#include "Soldiers/Players/SoldierPlayerState.h"
#include "SquadLeaderGameInstance.h"
#include "Soldiers/Soldier.h"

ASquadLeaderGameModeBase::ASquadLeaderGameModeBase() : RespawnDelay{ 3.f }
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("/Game/BluePrints/Soldiers/Players/BP_SoldierPlayerSupport"));
	static ConstructorHelpers::FClassFinder<ASoldierPlayerController> PlayerControllerObject(TEXT("/Game/BluePrints/Soldiers/Players/BP_SoldierPlayerController"));
	static ConstructorHelpers::FClassFinder<ASoldierPlayerState> PlayerStateObject(TEXT("/Game/BluePrints/Soldiers/Players/BP_SoldierPlayerState"));

	if (PlayerPawnObject.Class != NULL)
		DefaultPawnClass = PlayerPawnObject.Class;

	if (PlayerControllerObject.Class != NULL)
		PlayerControllerClass = PlayerControllerObject.Class;

	if (PlayerStateObject.Class != NULL)
		PlayerStateClass = PlayerStateObject.Class;
}

void ASquadLeaderGameModeBase::StartPlay() {
	for (auto team : SoldierTeamCollection) {  // clean all team data at the begining
		team.GetDefaultObject()->CleanSpawnPoints();
		team.GetDefaultObject()->CleanSoldierList();
	}

	ControlAreaManager.GetDefaultObject()->CleanControlAreaList();  // clean the list of all control area
	
	//Init for AI
	Cast<USquadLeaderGameInstance>(GetGameInstance())->InitAIManagers();
	Super::StartPlay();
}

void ASquadLeaderGameModeBase::SoldierDied(AController* _Controller)
{
	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;

	RespawnDelegate = FTimerDelegate::CreateUObject(this, &ASquadLeaderGameModeBase::RespawnSoldier, _Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}

void ASquadLeaderGameModeBase::RespawnSoldier(AController* _Controller)
{
	if (ASoldier* soldier = Cast<ASoldier>(_Controller->GetPawn()); soldier)
	{
		soldier->SetActorLocation(soldier->GetRespawnPoint());
		soldier->Respawn();
	}
}