#include "SquadLeaderGameModeBase.h"
#include "Net/OnlineEngineInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Soldiers/Players/SoldierPlayerController.h"
#include "Soldiers/Players/SoldierPlayerState.h"
#include "Soldiers/Soldier.h"
#include "AbilitySystemGlobals.h"
#include "Kismet/KismetMathLibrary.h"
#include "SquadLeaderGameInstance.h"
#include "MainMenu/GameParam/GameParam.h"

#include "GameState/SquadLeaderGameState.h"
#include "GameState/SquadLeaderInitGameState.h"
#include "GameState/SquadLeaderCloseGameState.h"



ASquadLeaderGameModeBase::ASquadLeaderGameModeBase()
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


void ASquadLeaderGameModeBase::Logout(AController* Exiting)
{
	// notifies that a player has left
	if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("A player left the game."));  // message for the server only
	// TODO: ensure that the player is correctly destroy

	// do the basic job
	Super::Logout(Exiting);
}


APawn* ASquadLeaderGameModeBase::SpawnSoldier(APlayerParam* PlayerParam, AController* OwningController)
{
	if (auto SLInitGameState = Cast<ASquadLeaderInitGameState>(GameState); SLInitGameState) {
		if (ASoldierTeam* NewSoldierTeam = SLInitGameState->GetSoldierTeamByID(PlayerParam->GetTeam()); NewSoldierTeam) {
			TArray<ASoldierSpawn*> SpawnList = NewSoldierTeam->GetUsableSpawnPoints();
			if (SpawnList.Num() > 0) {
				ASoldierSpawn* SpawnPoint = SpawnList[UKismetMathLibrary::RandomIntegerInRange(0, SpawnList.Num() - 1)];
				
				FTransform SpawnPosition = { SpawnPoint->GetActorRotation(), SpawnPoint->GetActorLocation() };
				APawn* NewPawn = GetWorld()->SpawnActorDeferred<APawn>(PlayerParam->GetPlayerSoldier(),
					SpawnPosition, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				
				if (NewPawn) {
					NewPawn->Controller = OwningController;
					Cast<ASoldier>(NewPawn)->SetTeam(NewSoldierTeam);
					//set AISquad composition

					NewPawn->FinishSpawning(SpawnPosition);
					return NewPawn;
				}
			}
		}
	}
	return nullptr;
}

void ASquadLeaderGameModeBase::ChangeGameState() {
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
	FetchGameParam();

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

	FTimerHandle ControlAreaTimer{};
	GetWorldTimerManager().SetTimer(ControlAreaTimer, this, &ASquadLeaderGameModeBase::CheckControlAreaAdvantage, FrequenceForControlAreaInfluence, true);

	Super::StartPlay();
}

void ASquadLeaderGameModeBase::FetchGameParam()
{
	// import game param from SquadLeaderGameInstance's GameParam object
	AGameParam* ImportedGameParam = GetGameInstance<USquadLeaderGameInstance>()->GameParam.GetDefaultObject();
	RespawnDelay = ImportedGameParam->RespawnDuration;
	Weather = ImportedGameParam->Weather;
	BaseTicketsNumber = ImportedGameParam->NbTickets;
	AIBasicAssaultNumber = ImportedGameParam->NbAIBasicAssault;
	AIBasicHeavyNumber = ImportedGameParam->NbAIBasicHeavy;
	AIBasicLevel = ImportedGameParam->LevelAIBasic;
	StartingAISquadNumber = ImportedGameParam->StartingNbAISquad;
	AISquadLevel = ImportedGameParam->LevelAISquad;
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

void ASquadLeaderGameModeBase::InitInfluenceMap() {
	FTransform LocationTemp{ {0.f, 0.f, 0.f}, {0.f,0.f,0.f} };
	AInfluenceMapGrid* _InfluenceMap = GetWorld()->SpawnActorDeferred<AInfluenceMapGrid>(InfluenceMapClass, LocationTemp, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (_InfluenceMap) {
		_InfluenceMap->FinishSpawning(LocationTemp);
		InfluenceMap = _InfluenceMap;
	}
}

void ASquadLeaderGameModeBase::CheckControlAreaAdvantage()
{
	if (AControlAreaManager* ControlAreaManager = Cast<ASquadLeaderInitGameState>(GameState)->GetControlAreaManager(); ControlAreaManager) {
		if (auto TeamWithAdvantage = ControlAreaManager->GetTeamWithControlAdvantage(); TeamWithAdvantage && (TeamWithAdvantage->Id == 1 || TeamWithAdvantage->Id == 2)) {
			// we need to find the other team
			if (auto TeamWithoutAdvantage = Cast<ASquadLeaderInitGameState>(GameState)->GetSoldierTeamByID((TeamWithAdvantage->Id % 2) + 1); TeamWithoutAdvantage) {
				TeamWithoutAdvantage->RemoveTickets(ControlAreaTicketsReduction);
			}
		}
	}
}

void ASquadLeaderGameModeBase::RespawnSoldier(ASoldier* _Soldier, AControlArea* _ControlArea)
{
	if (_Soldier)
	{
		_Soldier->SetActorLocation(_Soldier->GetRespawnPoint(_ControlArea));
		_Soldier->Respawn();
	}
}

void ASquadLeaderGameModeBase::CheckControlAreaVictoryCondition()
{
	if (auto WinnerTeam = Cast<ASquadLeaderGameState>(GameState)->GetControlAreaManager()->GetTeamWithAllControl(); WinnerTeam) {
		//GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, TEXT("END GAME: All control area taken\n") + WinnerTeam->TeamName + TEXT(" win !"), false, FVector2D(7, 7));
		EndGame(WinnerTeam);
	}
}

void ASquadLeaderGameModeBase::CheckTeamTicketsVictoryCondition()
{
	for (auto team : Cast<ASquadLeaderGameState>(GameState)->GetSoldierTeamCollection()) {
		if (team->GetTickets() <= 0) {
			//GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, TEXT("END GAME: Tickets depleted\n") + teams->TeamName + TEXT(" lose !"), false, FVector2D(7, 7));
			for (auto potentialwinningteam : Cast<ASquadLeaderGameState>(GameState)->GetSoldierTeamCollection())
				if (potentialwinningteam != team && (potentialwinningteam->Id == 1 || potentialwinningteam->Id == 2))
					EndGame(potentialwinningteam);
		}
	}
}

void ASquadLeaderGameModeBase::EndGame(ASoldierTeam* WinningTeam)
{
	if (!IsGameOver) {
		for (auto PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; PCIterator++)
		{
			if (auto PC = Cast<ASoldierPlayerController>(PCIterator->Get()); PC)
			{
				if (AKillStats* killRecord = PC->GetPlayerState<ASoldierPlayerState>()->PersonalRecord; killRecord) {
					if (PC->GetTeam() == WinningTeam) {
						PC->OnGameEnd(1, GetGameTimeSinceCreation(),
							killRecord->NbKillAI, killRecord->NbKillPlayer,
							killRecord->NbDeathByAI, killRecord->NbDeathByPlayer);
						for (ASoldier* Soldier : PC->GetTeam()->GetSoldierList())
						{
							if (ASoldierPlayer* Player = Cast<ASoldierPlayer>(Soldier); Player)
							Player->ClientNotifyEndGame(true);
						}
					}
					else {
						PC->OnGameEnd(-1, GetGameTimeSinceCreation(),
							killRecord->NbKillAI, killRecord->NbKillPlayer,
							killRecord->NbDeathByAI, killRecord->NbDeathByPlayer);
						for (ASoldier* Soldier : PC->GetTeam()->GetSoldierList())
						{
							if (ASoldierPlayer* Player = Cast<ASoldierPlayer>(Soldier); Player)
								Player->ClientNotifyEndGame(false);
						}
					}
				}
			}
		}

		if (ASquadLeaderInitGameState* GS = Cast<ASquadLeaderInitGameState>(GameState); GS)
		{
			for (ASoldierTeam* SoldierTeam : GS->GetSoldierTeamCollection())
			{
				for (ASoldier* Soldier : SoldierTeam->GetSoldierList())
					Soldier->LockControls();
			}
		}

		FTimerHandle timerBeforeClosing;
		GetWorld()->GetTimerManager().SetTimer(timerBeforeClosing, this,
			&ASquadLeaderGameModeBase::CloseGame, 10.f);  // request to the server to end the game

		IsGameOver = true;
	}
}

void ASquadLeaderGameModeBase::CloseGame()
{
	for (auto PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; PCIterator++)
	{
		if (auto PC = Cast<ASoldierPlayerController>(PCIterator->Get()); PC)
		{
			PC->ClientSendChangeMapCommand("open MapMainMenu");
		}
	}
	//FGenericPlatformMisc::RequestExit(false);
}

void ASquadLeaderGameModeBase::DisplayRespawnHUD(ASoldierPlayer* _SoldierPlayer)
{
	AControlAreaManager* ControlAreaManager = Cast<ASquadLeaderInitGameState>(GameState)->GetControlAreaManager();
	if (ControlAreaManager)
	{
		ASoldierPlayerController* PC = _SoldierPlayer->GetController<ASoldierPlayerController>();
		PC->ClientDisplayRespawnWidget();
	}
}

void ASquadLeaderGameModeBase::NotifySoldierKilled(ASoldier* _DeadSoldier, ASoldier* _Killer)
{
	UpdateTicketsFromSoldierDeath(_DeadSoldier);
	GrantEXPFromSoldierDeath(_DeadSoldier, _Killer);
	NotifySoldierDeathToAllPlayers(_DeadSoldier, _Killer);
	ManageKillingStreak(_DeadSoldier, _Killer);
	StartRespawnTimerForDeadSoldier(_DeadSoldier);
}

void ASquadLeaderGameModeBase::NotifyControlAreaCaptured(AControlArea* _ControlArea)
{
	ASoldierTeam* TeamOwner = _ControlArea->GetIsTakenBy();

	//TMap<ASoldierTeam*, AControlAreaTeamStat*> TeamData;
	for (auto Pair : _ControlArea->TeamData)
	{
		if (ASoldierTeam * SoldierTeam = Pair.Key; SoldierTeam == TeamOwner)
		{
			for (ASoldier* Soldier : SoldierTeam->GetSoldierList())
			{
				if (ASoldierPlayer* Player = Cast<ASoldierPlayer>(Soldier); Player)
					Player->ClientNotifyControlAreaTaken(true);
			}
		}
		else
		{
			for (ASoldier* Soldier : SoldierTeam->GetSoldierList())
			{
				if (ASoldierPlayer* Player = Cast<ASoldierPlayer>(Soldier); Player)
					Player->ClientNotifyControlAreaTaken(false);
			}
		}
	}



	if (TeamOwner)
	{
		for (ASoldier* Soldier : TeamOwner->GetSoldierList())
		{
			if (Soldier->IsA<ASoldierPlayer>())
				Soldier->GrantEXP(EXP_ControlAreaCaptured);
			
		}
		CheckControlAreaVictoryCondition();
	}
}

void ASquadLeaderGameModeBase::UpdateTicketsFromSoldierDeath(ASoldier* _DeadSoldier)
{
	ASoldierTeam* SoldierTeam = _DeadSoldier->GetTeam();
	if (SoldierTeam)
	{
		if (_DeadSoldier->IsA<ASoldierPlayer>())
			SoldierTeam->RemoveTickets(TicketToRemove_Player); // Soldier is a player
		else if (ASoldierAI* SoldierAI = Cast<ASoldierAI>(_DeadSoldier); SoldierAI)
		{
			if (SoldierAI->GetController()->IsA<AAISquadController>()) 
				SoldierTeam->RemoveTickets(TicketToRemove_AISquad); // Soldier is a squad AI
			else
				SoldierTeam->RemoveTickets(TicketToRemove_AIBasic); // Soldier is a basic AI
		}
		CheckTeamTicketsVictoryCondition();
	}
}


void ASquadLeaderGameModeBase::ManageKillingStreak(ASoldier* _DeadSoldier, ASoldier* _Killer)
{
	if (ASoldierPlayer* DeadSoldier = Cast<ASoldierPlayer>(_DeadSoldier); DeadSoldier) {
		ResetKillingStreak(DeadSoldier);
	}
	if (ASoldierPlayer* Killer = Cast<ASoldierPlayer>(_Killer); Killer) {
		IncreaseKillingStreak(Killer);
	}
	if (AAISquadController* SC = Cast<AAISquadController>(_Killer->GetController()); SC) {
		if (AAISquadManager* SquadManager = SC->SquadManager; SquadManager) {
			IncreaseKillingStreak(SquadManager->Leader);
		}
	}
}

void ASquadLeaderGameModeBase::ResetKillingStreak(ASoldierPlayer* _Soldier)
{
	if (ASoldierPlayerState* PS = _Soldier->GetPlayerState<ASoldierPlayerState>(); PS) {
		if (PS->KillingStreak >= NbKillsForBounty) {  // apply and notify bounty
			_Soldier->GetTeam()->RemoveTickets(BountyValueInTickets * (PS->KillingStreak / NbKillsForBounty));
			PS->IsinKillingStreak = false;
			NotifyBounty(_Soldier);
		}
		PS->KillingStreak = 0;
	}
}

void ASquadLeaderGameModeBase::NotifyBounty(ASoldierPlayer* _Soldier)
{
	for (auto PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; PCIterator++) {
		if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(PCIterator->Get()); PC) {
			PC->OnTextNotification_Received(FString::Printf(TEXT("%s's killing spree is over."), *_Soldier->GetName()));
		}
	}
}

void ASquadLeaderGameModeBase::IncreaseKillingStreak(ASoldierPlayer* _Soldier)
{
	if (ASoldierPlayerState* PS = _Soldier->GetPlayerState<ASoldierPlayerState>(); PS) {
		PS->KillingStreak ++;
		if (PS->KillingStreak % NbKillsForBounty == 0) {  // apply and notify bounty
			PS->IsinKillingStreak = true;
			NotifyKillingStreak(_Soldier);
		}
	}
}

void ASquadLeaderGameModeBase::NotifyKillingStreak(ASoldierPlayer* _Soldier)
{
	for (auto PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; PCIterator++) {
		if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(PCIterator->Get()); PC) {
			PC->OnTextNotification_Received(FString::Printf(TEXT("%s is in killing spree !"), *_Soldier->GetName()));
		}
	}
}


void ASquadLeaderGameModeBase::StartRespawnTimerForDeadSoldier(ASoldier* _DeadSoldier)
{
	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;

	if (ASoldierPlayer* SoldierPlayer = Cast<ASoldierPlayer>(_DeadSoldier))
		RespawnDelegate = FTimerDelegate::CreateUObject(this, &ASquadLeaderGameModeBase::DisplayRespawnHUD, SoldierPlayer);
	else
		RespawnDelegate = FTimerDelegate::CreateUObject(this, &ASquadLeaderGameModeBase::RespawnSoldier, _DeadSoldier, (AControlArea*)nullptr);
	
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);

	/*if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(_Controller); PC)
		PC->SetRespawnCountdown(RespawnDelay);*/
}

void ASquadLeaderGameModeBase::NotifySoldierDeathToAllPlayers(ASoldier* _DeadSoldier, ASoldier* _KillerSoldier)
{
	const ASoldierTeam* DeadSoldierTeam = _DeadSoldier->GetTeam();
	const ASoldierTeam* KillerSoldierTeam = _KillerSoldier->GetTeam();

	const ASoldierPlayer* DeadSoldierPlayer = Cast<ASoldierPlayer>(_DeadSoldier);
	const ASoldierPlayer* KillerSoldierPlayer = Cast<ASoldierPlayer>(_KillerSoldier);

	const bool bDeadIsPlayer = !!DeadSoldierPlayer;
	const bool bKillerIsPlayer = !!KillerSoldierPlayer;
	const bool bPlayerInvolved = bDeadIsPlayer || bKillerIsPlayer;
	const bool bIsSuicide = bKillerIsPlayer && (_DeadSoldier == _KillerSoldier);
	const bool bKillerIsSquadAI = _KillerSoldier->GetController()->IsA<AAISquadController>();

	// Leader of the killer - for AIs
	const ASoldier* SoldierLeader = nullptr;

	if (bKillerIsSquadAI)
	{
		const AAISquadManager* SquadManager = Cast<AAISquadController>(_KillerSoldier->GetController())->SquadManager;
		if (SquadManager)
			SoldierLeader = SquadManager->Leader;
	}

	for (auto PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; PCIterator++)
	{
		if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(PCIterator->Get()); PC)
		{
			// Ticket notification
			if (PC->GetTeam() == DeadSoldierTeam)
				PC->OnAllyTicket_Received(DeadSoldierTeam->GetTickets());
			else
				PC->OnEnnemyTicket_Received(DeadSoldierTeam->GetTickets());

			// Text notification
			if (bIsSuicide)
			{
				PC->OnTextNotification_Received(FString::Printf(TEXT("%s is dead..."), *_DeadSoldier->GetName()));
			}
			else if (bPlayerInvolved)
			{
				if (bKillerIsSquadAI && SoldierLeader)
					PC->OnTextNotification_Received(FString::Printf(TEXT(/*"%s from */"Squad of %s killed %s"), *_DeadSoldier->GetName(), *SoldierLeader->GetName()));
				else
					PC->OnTextNotification_Received(FString::Printf(TEXT("%s killed %s"), *_KillerSoldier->GetName(), *_DeadSoldier->GetName()));
			}
		}
	}
}

void ASquadLeaderGameModeBase::GrantOverTimeEXPToSoldier()
{
	for (ASoldierTeam* Team : Cast<ASquadLeaderGameState>(GameState)->GetSoldierTeamCollection())
	{
		for (ASoldier* Soldier : Team->GetSoldierList())
		{
			if (Soldier->IsA<ASoldierPlayer>())
				Soldier->GrantEXP(EXP_OverTime);
		}
	}
}

void ASquadLeaderGameModeBase::GrantEXPFromSoldierDeath(ASoldier* _DeadSoldier, ASoldier* _KillerSoldier)
{
	// Grant EXP to the killed player
	if (ASoldierPlayer* _DeadSoldierPlayer = Cast<ASoldierPlayer>(_DeadSoldier); _DeadSoldierPlayer) {
		_DeadSoldierPlayer->GrantEXP(EXP_Death);

		// Find if the killer is a soldier or an AI
		if (_KillerSoldier->IsA<ASoldierPlayer>()) {
			if (auto PS = _DeadSoldierPlayer->GetPlayerState<ASoldierPlayerState>(); PS) PS->PersonalRecord->NbDeathByPlayer++;
		}
		else {  // the killer is an AI
			if (auto PS = _DeadSoldierPlayer->GetPlayerState<ASoldierPlayerState>(); PS) PS->PersonalRecord->NbDeathByAI++;
		}
	}

	// Only Grant EXP to enemies
	if (_DeadSoldier->GetTeam() == _KillerSoldier->GetTeam())
		return;

	// Grant EXP to the killer player
	if (ASoldierPlayer* _KillerPlayer = Cast<ASoldierPlayer>(_KillerSoldier); _KillerPlayer)
	{
		_KillerPlayer->GrantEXP(EXP_Kill);

		// Find if the dead soldier is a soldier or an AI
		if (_DeadSoldier->IsA<ASoldierPlayer>()) {
			if (auto PS = _KillerPlayer->GetPlayerState<ASoldierPlayerState>(); PS) PS->PersonalRecord->NbKillPlayer++;
		}
		else {  // the dead soldier is an AI
			if (auto PS = _KillerPlayer->GetPlayerState<ASoldierPlayerState>(); PS) PS->PersonalRecord->NbKillAI++;
		}
	}
	// Grant EXP to the leader if the killer is a squad AI
	else if (AAISquadController* SquadController = Cast<AAISquadController>(_KillerSoldier->GetController()); SquadController && SquadController->SquadManager)
	{
		if (ASoldierPlayer* Leader = SquadController->SquadManager->Leader; Leader) {
			Leader->GrantEXP(EXP_KillSquad);
			if (_DeadSoldier->IsA<ASoldierPlayer>()) {
				if (auto PS = Leader->GetPlayerState<ASoldierPlayerState>(); PS) PS->PersonalRecord->NbKillPlayer++;
			}
			else {  // the dead soldier is an AI
				if (auto PS = Leader->GetPlayerState<ASoldierPlayerState>(); PS) PS->PersonalRecord->NbKillAI++;
			}
		}
	}
}