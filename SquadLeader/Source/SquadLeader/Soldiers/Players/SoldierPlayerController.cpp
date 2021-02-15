#include "SoldierPlayerController.h"
#include "SoldierPlayerState.h"
#include "../Soldier.h"
#include "AbilitySystemComponent.h"

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

	InputComponent->BindAxis("Turn", this, &ASoldierPlayerController::AddYawInput);
	InputComponent->BindAxis("LookUp", this, &ASoldierPlayerController::AddPitchInput);

	//InputComponent->BindAction("Run", IE_Pressed, this, &ASoldierPlayerController::onStartRunning);
	//InputComponent->BindAction("Run", IE_Released, this, &ASoldierPlayerController::onStopRunning);
}

void ASoldierPlayerController::onSwitchCamera()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->onSwitchCamera();
}

void ASoldierPlayerController::onMoveForward(const float _val)
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->onMoveForward(_val);
}

void ASoldierPlayerController::onMoveRight(const float _val) {

	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->onMoveRight(_val);
}

//void ASoldierPlayerController::onStartRunning()
//{
//	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
//		soldier->onStartRunning();
//}
//
//void ASoldierPlayerController::onStopRunning()
//{
//	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
//		soldier->onStopRunning();
//}