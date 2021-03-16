#include "SquadLeaderGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Soldiers/Players/SoldierPlayerController.h"
#include "Soldiers/Players/SoldierPlayerState.h"
#include "SquadLeaderGameInstance.h"
#include "Soldiers/Soldier.h"

#include "Interface/PreInitable.h"
#include "ControlArea/ControlArea.h"

ASquadLeaderGameModeBase::ASquadLeaderGameModeBase() : RespawnDelay{ 3.f }
{
	static ConstructorHelpers::FClassFinder<ASoldierPlayerController> PlayerControllerObject(TEXT("/Game/BluePrints/Soldiers/Players/BP_SoldierPlayerController"));
	static ConstructorHelpers::FClassFinder<ASoldierPlayerState> PlayerStateObject(TEXT("/Game/BluePrints/Soldiers/Players/BP_SoldierPlayerState"));

	if (PlayerControllerObject.Class != NULL)
		PlayerControllerClass = PlayerControllerObject.Class;

	if (PlayerStateObject.Class != NULL)
		PlayerStateClass = PlayerStateObject.Class;
}

void ASquadLeaderGameModeBase::StartPlay() {
	// Clean Managers and collections
	for (auto team : SoldierTeamCollection) {  // clean all team data at the begining
		team.GetDefaultObject()->CleanSpawnPoints();
		team.GetDefaultObject()->CleanSoldierList();
	}

	ControlAreaManager.GetDefaultObject()->CleanControlAreaList();  // clean the list of all control area

	//Init static world actors
	InitActorInWorld();
	
	//Init for AI
	Cast<USquadLeaderGameInstance>(GetGameInstance())->InitInfluenceMap();
	Cast<USquadLeaderGameInstance>(GetGameInstance())->InitAIManagers();

	Super::StartPlay();
}

void ASquadLeaderGameModeBase::InitActorInWorld()
{
	for (auto SceneActors: GetWorld()->PersistentLevel->Actors)  // cycle each actor
	{
		if (auto PreInitialisableObject = Cast<IPreInitable>(SceneActors); PreInitialisableObject)
			PreInitialisableObject->PreInitialisation();
	}
}

void ASquadLeaderGameModeBase::SoldierDied(AController* _Controller)
{
	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;

	RespawnDelegate = FTimerDelegate::CreateUObject(this, &ASquadLeaderGameModeBase::RespawnSoldier, _Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);

	/*if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(_Controller); PC)
		PC->SetRespawnCountdown(RespawnDelay);*/
}

void ASquadLeaderGameModeBase::RespawnSoldier(AController* _Controller)
{
	if (ASoldier* soldier = Cast<ASoldier>(_Controller->GetPawn()); soldier)
	{
		soldier->SetActorLocation(soldier->GetRespawnPoint());
		soldier->Respawn();
	}
}

void ASquadLeaderGameModeBase::CheckControlAreaVictoryCondition()
{
	if (auto WinnerTeam = ControlAreaManager.GetDefaultObject()->GetTeamWithAllControl(); WinnerTeam) {
		GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, TEXT("END GAME: All control area taken\n") + WinnerTeam.GetDefaultObject()->TeamName + TEXT(" win !"), false, FVector2D(7, 7));
		FTimerHandle timerBeforeClosing;
		GetWorld()->GetTimerManager().SetTimer(timerBeforeClosing, this,
			&ASquadLeaderGameModeBase::EndGame, 5.f);  // request to the server to end the game
	}
}

void ASquadLeaderGameModeBase::EndGame()
{
	for (auto Teams : SoldierTeamCollection) {
		for (auto Soldiers : Teams.GetDefaultObject()->soldierList) {
			if (auto PlayerControler = Cast<ASoldierPlayerController>(Soldiers->GetController()); PlayerControler) {
				PlayerControler->ClientSendCommand("EXIT", true);
			}
		}
	}
	//FGenericPlatformMisc::RequestExit(false);
}
