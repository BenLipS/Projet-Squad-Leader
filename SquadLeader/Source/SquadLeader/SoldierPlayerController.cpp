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

	InputComponent->BindAxis("MoveForward", this, &ASoldierPlayerController::onMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASoldierPlayerController::MoveRight);

	InputComponent->BindAxis("Turn", this, &ASoldierPlayerController::AddYawInput);
	InputComponent->BindAxis("LookUp", this, &ASoldierPlayerController::AddPitchInput);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ASoldierPlayerController::OnStartJumping);
	InputComponent->BindAction("Jump", IE_Released, this, &ASoldierPlayerController::OnStopJumping);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &ASoldierPlayerController::OnStartCrouching);
	InputComponent->BindAction("Crouch", IE_Released, this, &ASoldierPlayerController::OnStopCrouching);

	InputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ASoldierPlayerController::OnSwitchCamera);
}

void ASoldierPlayerController::OnSwitchCamera()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()))
		soldier->OnSwitchCamera();
}

void ASoldierPlayerController::onMoveForward(const float _val)
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()))
		soldier->onMoveForward(_val);
}

void ASoldierPlayerController::MoveRight(const float _val) {

	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()))
		soldier->onMoveRight(_val);
}

void ASoldierPlayerController::OnStartJumping()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()))
		soldier->OnStartJumping();
}

void ASoldierPlayerController::OnStopJumping()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()))
		soldier->OnStopJumping();
}

void ASoldierPlayerController::OnStartCrouching()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()))
		soldier->OnStartCrouching();
}

void ASoldierPlayerController::OnStopCrouching()
{
	if (ASoldier* soldier = Cast<ASoldier>(K2_GetPawn()))
		soldier->OnStopCrouching();
}