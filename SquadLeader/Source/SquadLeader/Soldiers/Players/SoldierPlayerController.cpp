#include "SoldierPlayerController.h"
#include "SoldierPlayerState.h"
#include "../Soldier.h"
#include "AbilitySystemComponent.h"
#include "../../UI/PlayerHUD.h"

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

void ASoldierPlayerController::CreateHUD()
{
	if (!HUDClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing HUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}
	ClientSetHUD(HUDClass);

	ASoldierPlayerState* PS = GetPlayerState<ASoldierPlayerState>();
	if (!PS)
		return;

	auto PlayerHUD = Cast<APlayerHUD>(MyHUD);

	// Player stats
	PlayerHUD->OnMaxHealthChanged(PS->GetMaxHealth());
	PlayerHUD->OnHealthChanged(PS->GetHealth());
	/*PlayerHUD->SetMaxShield(PS->GetMaxShield());
	PlayerHUD->SetShield(PS->GetShield());*/
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
}

void ASoldierPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Soldier is possessed.
	//CreateHUD();
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