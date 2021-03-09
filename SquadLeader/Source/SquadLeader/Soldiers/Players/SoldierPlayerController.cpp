#include "SoldierPlayerController.h"
#include "SoldierPlayerState.h"
#include "../Soldier.h"
#include "AbilitySystemComponent.h"

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
}

void ASoldierPlayerController::createHUD()
{
	if (HUDWidget || !IsLocalPlayerController()) // We only want the HUD in local
		return;

	if (!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing HUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}

	HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport();
}

UUserWidget* ASoldierPlayerController::getHUD() const
{
	return HUDWidget;
}

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


void ASoldierPlayerController::ClientSendCommand_Implementation(const FString& Cmd, bool bWriteToLog)
{
	ConsoleCommand(Cmd, bWriteToLog);
}
