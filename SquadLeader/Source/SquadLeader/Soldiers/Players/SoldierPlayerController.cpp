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
}

void ASoldierPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Soldier is possessed.
	CreateHUD();
	if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>())
	{
		CurrentPlayerHUD->SetPlayerStateLink();
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
	InputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ASoldierPlayerController::onSwitchCamera);

	// Movements
	InputComponent->BindAxis("MoveForward", this, &ASoldierPlayerController::onMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASoldierPlayerController::onMoveRight);

	InputComponent->BindAxis("Turn", this, &ASoldierPlayerController::onTurn);
	InputComponent->BindAxis("LookUp", this, &ASoldierPlayerController::onLookUp);

	//TODO : change debug bindAction when not need anymore
	InputComponent->BindAction("ChangeTeam", IE_Released, this, &ASoldierPlayerController::onChangeTeam);
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

void ASoldierPlayerController::onSwitchCamera()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->onSwitchCamera();
}

void ASoldierPlayerController::onMoveForward(const float _Val)
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->MoveForward(_Val);
}

void ASoldierPlayerController::onMoveRight(const float _Val) {

	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->MoveRight(_Val);
}

void ASoldierPlayerController::onLookUp(const float _Val)
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->LookUp(_Val);
}

void ASoldierPlayerController::onTurn(const float _Val)
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->Turn(_Val);
}

void ASoldierPlayerController::onChangeTeam()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->cycleBetweenTeam();
}