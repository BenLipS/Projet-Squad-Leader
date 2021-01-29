#include "Soldier.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"

ASoldier::ASoldier()
{
	PrimaryActorTick.bCanEverTick = true;

	initStats();
	initCameras();
	initMeshes();
	initMovements();
}

void ASoldier::BeginPlay()
{
	Super::BeginPlay();

	if (bIsFirstPerson)
		setToFirstCameraPerson();
	else
		setToThirdCameraPerson();
}

void ASoldier::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	//DOREPLIFETIME_CONDITION(ASoldier, Inventory, COND_OwnerOnly);

	// everyone except local owner: flag change is locally instigated
	DOREPLIFETIME_CONDITION(ASoldier, bWantsToRun, COND_SkipOwner);

	// everyone
	//DOREPLIFETIME(AShooterCharacter, CurrentWeapon);
}

void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASoldier::initCameras()
{
	// 1st person camera
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(2.f, 0.f, BaseEyeHeight));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetFieldOfView(fieldOfViewNormal);

	// 3rd person camera
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetCapsuleComponent());
	SpringArmComponent->TargetArmLength = 170;
	SpringArmComponent->SetRelativeLocation(FVector(-30.f, 40.f, BaseEyeHeight));
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraLagSpeed = 10.0f;
	SpringArmComponent->CameraRotationLagSpeed = 10.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArmComponent);
	ThirdPersonCameraComponent->SetFieldOfView(fieldOfViewNormal);

	bIsFirstPerson = true;
}

void ASoldier::initMeshes()
{
	// 1st person mesh
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetOnlyOwnerSee(true); // Only the player sees it
	FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;

	// 3rd person mesh - already defined with ACharacter
}

void ASoldier::initStats()
{
	fieldOfViewNormal = 90.f;
	fieldOfViewAim = 50.f;
}

void ASoldier::initMovements()
{
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200;
}

void ASoldier::onSwitchCamera()
{
	if (bIsFirstPerson)
		setToThirdCameraPerson();
	else
		setToFirstCameraPerson();
}

void ASoldier::setToFirstCameraPerson()
{
	ThirdPersonCameraComponent->Deactivate();
	GetMesh()->SetOwnerNoSee(true);
	FirstPersonCameraComponent->Activate();
	FirstPersonMesh->SetOwnerNoSee(false);

	bIsFirstPerson = true;
}

void ASoldier::setToThirdCameraPerson()
{
	FirstPersonCameraComponent->Deactivate();
	FirstPersonMesh->SetOwnerNoSee(true);
	ThirdPersonCameraComponent->Activate();
	GetMesh()->SetOwnerNoSee(false);

	bIsFirstPerson = false;
}

void ASoldier::onMoveForward(const float _val)
{
	if ((Controller != NULL) && (_val != 0.0f))
	{
		FRotator Rotation = Controller->GetControlRotation();

		// Ignore pitch
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
			Rotation.Pitch = 0.0f;

		AddMovementInput(FRotationMatrix(Rotation).GetScaledAxis(EAxis::X), _val);
	}
}

void ASoldier::onMoveRight(const float _val) {
	if ((Controller != NULL) && (_val != 0.0f))
	{
		FRotator Rotation = Controller->GetControlRotation();
		AddMovementInput(FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y), _val);
	}
}

void ASoldier::onStartJumping()
{
	Jump();
}

void ASoldier::onStopJumping()
{
	StopJumping();
}

void ASoldier::onStartCrouching()
{
	Crouch();
}

void ASoldier::onStopCrouching()
{
	UnCrouch();
}

void ASoldier::onStartRunning()
{
	setRunning(true);
}

void ASoldier::onStopRunning()
{
	setRunning(false);
}

void ASoldier::setRunning(const bool _wantsToRun)
{
	bWantsToRun = _wantsToRun;
	GetCharacterMovement()->MaxWalkSpeed = bWantsToRun ? 2200.f : 600.f;

	if (GetLocalRole() < ROLE_Authority)
		ServerSetRunning(bWantsToRun);
}

bool ASoldier::ServerSetRunning_Validate(const bool _wantsToRun)
{
	return true;
}

void ASoldier::ServerSetRunning_Implementation(const bool _wantsToRun)
{
	setRunning(_wantsToRun);
}

bool ASoldier::isRunning() const noexcept
{
	return bWantsToRun && !GetVelocity().IsZero();
}