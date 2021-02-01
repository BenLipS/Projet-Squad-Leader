#include "SoldierPlayerController.h"
#include "Soldier.h"

ASoldierPlayerController::ASoldierPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASoldierPlayerController::BeginPlay()
{
	Super::BeginPlay();
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

	InputComponent->BindAction("Jump", IE_Pressed, this, &ASoldierPlayerController::onStartJumping);
	InputComponent->BindAction("Jump", IE_Released, this, &ASoldierPlayerController::onStopJumping);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &ASoldierPlayerController::onStartCrouching);
	InputComponent->BindAction("Crouch", IE_Released, this, &ASoldierPlayerController::onStopCrouching);

	InputComponent->BindAction("Run", IE_Pressed, this, &ASoldierPlayerController::onStartRunning);
	InputComponent->BindAction("Run", IE_Released, this, &ASoldierPlayerController::onStopRunning);
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

void ASoldierPlayerController::onStartJumping()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->onStartJumping();
}

void ASoldierPlayerController::onStopJumping()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->onStopJumping();
}

void ASoldierPlayerController::onStartCrouching()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->onStartCrouching();
}

void ASoldierPlayerController::onStopCrouching()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->onStopCrouching();
}

void ASoldierPlayerController::onStartRunning()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->onStartRunning();
}

void ASoldierPlayerController::onStopRunning()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()); soldier)
		soldier->onStopRunning();
}