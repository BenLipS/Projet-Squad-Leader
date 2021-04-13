#include "SquadLeaderGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Soldiers/Players/SoldierPlayerController.h"
#include "Soldiers/Players/SoldierPlayerState.h"
#include "SquadLeaderGameInstance.h"
#include "Soldiers/Soldier.h"

#include "Interface/PreInitable.h"
#include "GameState/SquadLeaderGameState.h"
#include "GameState/SquadLeaderInitGameState.h"
#include "GameState/SquadLeaderCloseGameState.h"
#include "AbilitySystemGlobals.h"


ASquadLeaderGameModeBase::ASquadLeaderGameModeBase() : RespawnDelay{ 3.f }
{
	static ConstructorHelpers::FClassFinder<ASoldierPlayerController> PlayerControllerObject(TEXT("/Game/BluePrints/Soldiers/Players/BP_SoldierPlayerController"));
	static ConstructorHelpers::FClassFinder<ASoldierPlayerState> PlayerStateObject(TEXT("/Game/BluePrints/Soldiers/Players/BP_SoldierPlayerState"));

	if (PlayerControllerObject.Class != NULL)
		PlayerControllerClass = PlayerControllerObject.Class;

	if (PlayerStateObject.Class != NULL)
		PlayerStateClass = PlayerStateObject.Class;

	ListAISquadManagers = {};
	UAbilitySystemGlobals::Get().InitGlobalData();
}

void ASquadLeaderGameModeBase::InitGameWithGameState() {
	// set parameters for GameState's spawn
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	
	// change gameState
	GameStateClass = ASquadLeaderInitGameState::StaticClass();

	// spawn and set GameState
	UWorld* World = GetWorld();
	GameState = World->SpawnActor<ASquadLeaderInitGameState>(GameStateClass, SpawnInfo);
	World->SetGameState(GameState);
	if (GameState)
	{
		GameState->AuthorityGameMode = this;
	}

	InitGameState();
}


void ASquadLeaderGameModeBase::StartPlay() {
	// Set the GameState
	//InitGameWithGameState();

	// Launch the game initialization from the GameState 
	if (auto SLInitGameState = Cast<ASquadLeaderInitGameState>(GameState); SLInitGameState) {
		SLInitGameState->InitMapAndData();
	}
	
	//Init for AI
	InitInfluenceMap();
	InitAIManagers();

	// Start timer for the granted EXP over time
	FTimerHandle Handle{};
	GetWorldTimerManager().SetTimer(Handle, this, &ASquadLeaderGameModeBase::GrantOverTimeEXPToSoldier, TimeBetweenGrantedEXP, true);

	Super::StartPlay();
}


void ASquadLeaderGameModeBase::InitAIManagers()
{
	//if(GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Game Instance InitAIManagers()"));
	auto GS = Cast<ASquadLeaderInitGameState>(GameState);

	/*Init AIBasic Manager*/
	FTransform LocationTemp{ {0.f, -1000.f, 0.f}, {0.f,0.f,0.f} };
	for (auto team : GS->GetSoldierTeamCollection()) {
		AAIBasicManager* AIBasicManager = GetWorld()->SpawnActorDeferred<AAIBasicManager>(AIBasicManagerClass, LocationTemp, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (AIBasicManager) {
			AIBasicManager->FinishSpawning(LocationTemp);
			AIBasicManagerCollection.Add(team, AIBasicManager);
			AIBasicManagerCollection[team]->Init(team);
		}
	}

	/*Init AISquad Manager*/
	//Each Player init a SquadManager in SoldierPlayer.cpp PossessedBy
}

void ASquadLeaderGameModeBase::AddAIBasicToManager(AAIBasicController* AIBasic)
{
	auto GS = Cast<ASquadLeaderInitGameState>(GameState);
	if (AIBasic && AIBasic->GetTeam()) {
		if (auto FoundAIBasicManager = AIBasicManagerCollection.Find(AIBasic->GetTeam()); FoundAIBasicManager) {
			(*FoundAIBasicManager)->AIBasicList.Add(AIBasic);
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString{ "AIBasic " + AIBasic->GetTeam()->TeamName + " added"});
		}
		else {
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Une AI n'a pas d'equipe"));
		}
	}
	else {
		//if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Spawned thought the manager"));
	}
}

void ASquadLeaderGameModeBase::InitInfluenceMap() {
	FTransform LocationTemp{ {0.f, 0.f, 0.f}, {0.f,0.f,0.f} };
	AInfluenceMapGrid* _InfluenceMap = GetWorld()->SpawnActorDeferred<AInfluenceMapGrid>(InfluenceMapClass, LocationTemp, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (_InfluenceMap) {
		_InfluenceMap->FinishSpawning(LocationTemp);
		InfluenceMap = _InfluenceMap;
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
	if (auto WinnerTeam = Cast<ASquadLeaderGameState>(GameState)->GetControlAreaManager()->GetTeamWithAllControl(); WinnerTeam) {
		GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, TEXT("END GAME: All control area taken\n") + WinnerTeam->TeamName + TEXT(" win !"), false, FVector2D(7, 7));
		FTimerHandle timerBeforeClosing;
		GetWorld()->GetTimerManager().SetTimer(timerBeforeClosing, this,
			&ASquadLeaderGameModeBase::EndGame, 5.f);  // request to the server to end the game
	}
}

void ASquadLeaderGameModeBase::CheckTeamTicketsVictoryCondition()
{
	for (auto teams : Cast<ASquadLeaderGameState>(GameState)->GetSoldierTeamCollection()) {
		if (teams->GetTicket() == 0) {
			GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, TEXT("END GAME: Tickets depleted\n") + teams->TeamName + TEXT(" lose !"), false, FVector2D(7, 7));
			FTimerHandle timerBeforeClosing;
			GetWorld()->GetTimerManager().SetTimer(timerBeforeClosing, this,
				&ASquadLeaderGameModeBase::EndGame, 5.f);  // request to the server to end the game
		}
	}
}

void ASquadLeaderGameModeBase::EndGame()
{
	for (auto Teams : Cast<ASquadLeaderGameState>(GameState)->GetSoldierTeamCollection()) {
		for (auto Soldiers : Teams->GetSoldierList()) {
			if (auto PlayerControler = Cast<ASoldierPlayerController>(Soldiers->GetController()); PlayerControler) {
				PlayerControler->ClientSendCommand("EXIT", true);
			}
		}
	}
	//FGenericPlatformMisc::RequestExit(false);
}

void ASquadLeaderGameModeBase::GrantOverTimeEXPToSoldier()
{
	for (ASoldierTeam* Team : Cast<ASquadLeaderGameState>(GameState)->GetSoldierTeamCollection())
	{
		for (ASoldier* Soldier : Team->GetSoldierList())
		{
			if (Soldier->IsA<ASoldierPlayer>())
				Soldier->GetAttributeSet()->GrantEXP(EXP_OverTime);
		}
	}
}

void ASquadLeaderGameModeBase::NotifySoldierKilled(ASoldier* _DeadSoldier, ASoldier* _Killer)
{
	// Grant EXP to the killed player
	if (ASoldierPlayer* _DeadSoldierPlayer = Cast<ASoldierPlayer>(_DeadSoldier); _DeadSoldierPlayer)
		_DeadSoldierPlayer->GetAttributeSet()->GrantEXP(EXP_Death);

	// Grant EXP to the killer player
	if (ASoldierPlayer* _KillerPlayer = Cast<ASoldierPlayer>(_Killer); _KillerPlayer)
	{
		_Killer->GetAttributeSet()->GrantEXP(EXP_Kill);
	}
	// Grant EXP to the leader if the killer is a squad AI
	else if (ASoldierAI* _KillerAI = Cast<ASoldierAI>(_Killer); _KillerAI) 
	{
		AAISquadController* SquadController = Cast<AAISquadController>(_KillerAI->GetController());
		if (SquadController && SquadController->SquadManager)
		{
			if (ASoldierPlayer* Leader = SquadController->SquadManager->Leader; Leader)
				Leader->GetAttributeSet()->GrantEXP(EXP_KillSquad);
		}
	}
}

void ASquadLeaderGameModeBase::NotifyControlAreaCaptured(AControlArea* _ControlArea)
{
	if (ASoldierTeam* Team = _ControlArea->IsTakenBy; Team)
	{
		for (ASoldier* Soldier : Team->GetSoldierList())
		{
			if (Soldier->IsA<ASoldierPlayer>())
				Soldier->GetAttributeSet()->GrantEXP(EXP_ControlAreaCaptured);
		}
		CheckControlAreaVictoryCondition();
	}
}