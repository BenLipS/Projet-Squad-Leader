#include "SoldierPlayerController.h"
#include "SoldierPlayerState.h"
#include "../Soldier.h"
#include "AbilitySystemComponent.h"
#include "../../UI/PlayerHUD.h"

//TODO: rmove next include -> only use for the team init -> only use on temporary debug
#include "../../SquadLeaderGameModeBase.h"
#include "../Players/SoldierPlayer.h"
#include "../../AI/AISquadManager.h"

ASoldierPlayerController::ASoldierPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SquadManagerData = FAISquadManagerData();
}

void ASoldierPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CreateHUD();
}

void ASoldierPlayerController::CreateHUD_Implementation()
{
	if (!HUDClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing HUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}
	APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>();
	if (CurrentPlayerHUD)
		return;
	ClientSetHUD(HUDClass);
}


/*UHUDWidget* ASoldierPlayerController::GetHUD() const
{
	return HUDWidget;
}*/

/*void ASoldierPlayerController::SetRespawnCountdown_Implementation(const float _RespawnTimeRemaining)
{
	/*if (HUDWidget)
		HUDWidget->SetRespawnCountdown(_RespawnTimeRemaining);
}

bool ASoldierPlayerController::SetRespawnCountdown_Validate(const float _RespawnTimeRemaining)
{
	return true;
}*/

// Server only
void ASoldierPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ASoldierPlayerState* PS = GetPlayerState<ASoldierPlayerState>(); PS)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);

	//TODO: remove the team init -> only use on temporary debug
	if (auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); gameMode) {
		SetTeam(gameMode->SoldierTeamCollection[0]);
		if (auto soldier = Cast<ASoldierPlayer>(InPawn); soldier->GetSquadManager()) {
			soldier->GetSquadManager()->UpdateSquadTeam(GetTeam());
		}
	}
	Cast<ASoldierPlayer>(InPawn)->GetSquadManager()->OnSquadChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadChanged);
	Cast<ASoldierPlayer>(InPawn)->GetSquadManager()->OnMemberHealthChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberHealthChanged);
	Cast<ASoldierPlayer>(InPawn)->GetSquadManager()->OnMemberMaxHealthChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberMaxHealthChanged);
	Cast<ASoldierPlayer>(InPawn)->GetSquadManager()->BroadCastSquadData();
}

void ASoldierPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Soldier is possessed.
	CreateHUD();
	if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>())
	{
		CurrentPlayerHUD->SetPlayerStateLink();
		CurrentPlayerHUD->SetAIStateLink();
	}
}

void ASoldierPlayerController::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}

void ASoldierPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	// Cameras
	InputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ASoldierPlayerController::OnSwitchCamera);

	// Movements
	InputComponent->BindAxis("MoveForward", this, &ASoldierPlayerController::OnMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASoldierPlayerController::OnMoveRight);

	InputComponent->BindAxis("Turn", this, &ASoldierPlayerController::OnTurn);
	InputComponent->BindAxis("LookUp", this, &ASoldierPlayerController::OnLookUp);

	//TODO : change debug bindAction when not need anymore
	InputComponent->BindAction("ChangeTeam", IE_Released, this, &ASoldierPlayerController::OnChangeTeam);
}

TSubclassOf<ASoldierTeam> ASoldierPlayerController::GetTeam()
{
	if (auto SoldierState = Cast<ASoldierPlayerState>(PlayerState); SoldierState) {
		return SoldierState->GetTeam();
	}
	return nullptr;
}

bool ASoldierPlayerController::SetTeam(TSubclassOf<ASoldierTeam> _Team)
{
	if (auto SoldierState = Cast<ASoldierPlayerState>(PlayerState); SoldierState) {
		return SoldierState->SetTeam(_Team);
	}
	return false;
}

void ASoldierPlayerController::OnSwitchCamera()
{
	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->onSwitchCamera();
}

void ASoldierPlayerController::OnMoveForward(const float _Val)
{
	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->MoveForward(_Val);
}

void ASoldierPlayerController::OnMoveRight(const float _Val) {

	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->MoveRight(_Val);
}

void ASoldierPlayerController::OnLookUp(const float _Val)
{
	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->LookUp(_Val);
}

void ASoldierPlayerController::OnTurn(const float _Val)
{
	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->Turn(_Val);
}

void ASoldierPlayerController::OnChangeTeam()
{
	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->cycleBetweenTeam();
}


void ASoldierPlayerController::ClientSendCommand_Implementation(const FString& Cmd, bool bWriteToLog)
{
	ConsoleCommand(Cmd, bWriteToLog);
}

void ASoldierPlayerController::OnSquadChanged_Implementation(const TArray<FSoldierAIData>& newValue)
{
	SquadManagerData.OnSquadDataChanged(newValue);
	if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>(); CurrentPlayerHUD)
		CurrentPlayerHUD->OnSquadChanged(newValue);
}

void ASoldierPlayerController::OnSquadMemberHealthChanged_Implementation(int index, float newHealth)
{
	if (SquadManagerData.SquadData.IsValidIndex(index))
	{
		SquadManagerData.SquadData[index].Health = newHealth;
		//Appel au HUD
		if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>(); CurrentPlayerHUD)
		{
			CurrentPlayerHUD->OnSquadHealthChanged(index, newHealth);
		}
	}
	// Erreur syncronisation client / serveur
}

void ASoldierPlayerController::OnSquadMemberMaxHealthChanged_Implementation(int index, float newMaxHealth)
{
	if (SquadManagerData.SquadData.IsValidIndex(index))
	{
		SquadManagerData.SquadData[index].MaxHealth = newMaxHealth;
		//Appel au HUD
		if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>(); CurrentPlayerHUD)
		{
			CurrentPlayerHUD->OnSquadMaxHealthChanged(index, newMaxHealth);
		}
	}
	// Erreur syncronisation client / serveur
}

void ASoldierPlayerController::BroadCastManagerData()
{
	if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>(); CurrentPlayerHUD)
		CurrentPlayerHUD->OnSquadChanged(SquadManagerData.SquadData);
}