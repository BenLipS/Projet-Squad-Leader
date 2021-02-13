#include "Soldier.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "SoldierPlayerState.h"
#include "SoldierPlayerController.h"
#include "AIController.h"
#include "../../AbilitySystem/Soldiers/GameplayAbilitySoldier.h"
//#include "DrawDebugHelpers.h"

ASoldier::ASoldier() : bAbilitiesInitialized{ false }, ASCInputBound{ false }, bDefaultWeaponsInitialized{ false }
{
	PrimaryActorTick.bCanEverTick = true;

	initStats();
	initCameras();
	initMovements();
	initMeshes();
}

/*
* On the Server, Possession happens before BeginPlay.
* On the Client, BeginPlay happens before Possession.
* So we can't use BeginPlay to do anything with the AbilitySystemComponent because we don't have it until the PlayerState replicates from possession.
*/
void ASoldier::BeginPlay()
{
	Super::BeginPlay();

	if (bIsFirstPerson)
		setToFirstCameraPerson();
	else
		setToThirdCameraPerson();
}

void ASoldier::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	SetAbilitySystemComponent();
	initWeapons();

	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetController()); PC)
		PC->createHUD();
}

void ASoldier::PossessedBy(AController* _newController)
{
	Super::PossessedBy(_newController);
	SetAbilitySystemComponent();
	initWeapons();

	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetController()); PC)
		PC->createHUD();}

void ASoldier::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	//DOREPLIFETIME_CONDITION(ASoldier, Inventory, COND_OwnerOnly);

	// everyone except local owner: flag change is locally instigated

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
	CurrentCameraComponent = FirstPersonCameraComponent;
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
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
}

void ASoldier::initWeapons()
{
	if (bDefaultWeaponsInitialized)
		return;

	for (int32 i = 0; i < DefaultWeaponClasses.Num(); ++i)
	{
		if (DefaultWeaponClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.Instigator = GetInstigator();
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AWeapon* weapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClasses[i], SpawnInfo);

			if (weapon)
			{
				addToInventory(weapon);
				weapon->InitializeAbilitySystemComponent(AbilitySystemComponent);
			}
		}
	}

	if (Inventory.Num() > 0)
		currentWeapon = Inventory[0];

	bDefaultWeaponsInitialized = true;
}

UAbilitySystemSoldier* ASoldier::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSetSoldier* ASoldier::GetAttributeSet() const
{
	return AttributeSet;
}

void ASoldier::SetAbilitySystemComponent()
{
	// TODO: si pas de playerState alors IA donc intiliaser avec les attributs ability system de SOldier au lieu du playerState
	if (!IsValid(GetPlayerState()))
		return;

	if (ASoldierPlayerState* PS = Cast<ASoldierPlayerState>(GetPlayerState()); PS)
	{
		AbilitySystemComponent = PS->GetAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		AttributeSet = PS->GetAttributeSet();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();
		InitializeAbilities();
		AddStartupEffects();
		InitializeTagChangeCallbacks();
		BindASCInput();
	}
}

void ASoldier::BindASCInput()
{
	ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetController());
	
	if (!PC)
		return;

	UInputComponent* inputComponent = PC->InputComponent;

	if (!ASCInputBound && AbilitySystemComponent && IsValid(inputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(inputComponent, FGameplayAbilityInputBinds(FString("Confirm"),
			FString("Cancel"), FString("ESoldierAbilityInputID"), static_cast<int32>(ESoldierAbilityInputID::Confirm), static_cast<int32>(ESoldierAbilityInputID::Cancel)));

		ASCInputBound = true;
	}
}

void ASoldier::InitializeAttributes()
{
	if (!AbilitySystemComponent || !DefaultAttributeEffects)
		return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffects, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
	}
}

void ASoldier::InitializeAbilities()
{
	check(AbilitySystemComponent);

	if (GetLocalRole() == ROLE_Authority && !bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server
		for (TSubclassOf<UGameplayAbilitySoldier>& StartupAbility : CharacterDefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetCharacterLevel(), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
		}
		bAbilitiesInitialized = true;
	}
}

void ASoldier::AddStartupEffects()
{
	check(AbilitySystemComponent);

	if (GetLocalRole() == ROLE_Authority && !AbilitySystemComponent->startupEffectsApplied)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
		{
			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		AbilitySystemComponent->startupEffectsApplied = true;
	}
}

void ASoldier::InitializeTagChangeCallbacks()
{
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Fighting")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASoldier::FightingTagChanged);
}

void ASoldier::FightingTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
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

	CurrentCameraComponent = FirstPersonCameraComponent;
	bIsFirstPerson = true;
}

void ASoldier::setToThirdCameraPerson()
{
	FirstPersonCameraComponent->Deactivate();
	FirstPersonMesh->SetOwnerNoSee(true);
	ThirdPersonCameraComponent->Activate();
	GetMesh()->SetOwnerNoSee(false);

	CurrentCameraComponent = ThirdPersonCameraComponent;
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

FVector ASoldier::lookingAtPosition()
{
	// TODO: Handle AIs
	FHitResult outHit;

	FVector startLocation = CurrentCameraComponent->GetComponentTransform().GetLocation();
	FVector forwardVector = CurrentCameraComponent->GetForwardVector();
	FVector endLocation = startLocation + forwardVector * 10000.f;

	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(outHit, startLocation, endLocation, ECollisionChannel::ECC_WorldStatic, collisionParams))
		return outHit.bBlockingHit ? outHit.Location : endLocation;
	return endLocation;
}

int32 ASoldier::GetCharacterLevel() const
{
	if (AttributeSet)
		return static_cast<int32>(AttributeSet->GetCharacterLevel());
	return -1;
}

float ASoldier::GetHealth() const
{
	return AttributeSet ? AttributeSet->GetHealth() : -1.0f;
}

float ASoldier::GetMaxHealth() const
{
	return AttributeSet ? AttributeSet->GetMaxHealth() : -1.0f;
}

bool ASoldier::IsAlive() const
{
	return GetHealth() > 0.0f;
}

float ASoldier::GetMoveSpeed() const
{
	return AttributeSet ? AttributeSet->GetMoveSpeed() : -1.0f;
}

bool ASoldier::GetWantsToFire() const
{
	return wantsToFire;
}

void ASoldier::SetWantsToFire(const bool _want)
{
	wantsToFire = _want;
	if (wantsToFire) {
		currentWeapon->tryFiring();
	}
}

void ASoldier::SetWantsToFire(const bool _want, const FGameplayEffectSpecHandle _damageEffectSpecHandle)
{
	wantsToFire = _want;
	if (wantsToFire) {
		currentWeapon->tryFiring(_damageEffectSpecHandle);
	}
}


void ASoldier::OnRep_CurrentWeapon(AWeapon* _lastWeapon)
{
	SetCurrentWeapon(currentWeapon, _lastWeapon);
}

void ASoldier::addToInventory(AWeapon* _weapon)
{
	Inventory.Add(_weapon);
}

void ASoldier::SetCurrentWeapon(AWeapon* _newWeapon, AWeapon* _previousWeapon)
{
	if (_previousWeapon && _newWeapon !=_previousWeapon)
		currentWeapon = _newWeapon;
}
