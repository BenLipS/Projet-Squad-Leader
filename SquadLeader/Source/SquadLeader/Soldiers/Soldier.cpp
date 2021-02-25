#include "Soldier.h"
#include "SoldierMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "../AbilitySystem/Soldiers/GameplayAbilitySoldier.h"
#include "../AbilitySystem/Soldiers/GameplayEffects/States/GE_StateDead.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "../SquadLeaderGameModeBase.h"
//#include "DrawDebugHelpers.h"

// States
FGameplayTag ASoldier::StateDeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
FGameplayTag ASoldier::StateRunningTag = FGameplayTag::RequestGameplayTag(FName("State.Running"));
FGameplayTag ASoldier::StateJumpingTag = FGameplayTag::RequestGameplayTag(FName("State.Jumping"));
FGameplayTag ASoldier::StateCrouchingTag = FGameplayTag::RequestGameplayTag(FName("State.Crouching"));
FGameplayTag ASoldier::StateFightingTag = FGameplayTag::RequestGameplayTag(FName("State.Fighting"));

// Abilities
FGameplayTag ASoldier::SkillRunTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Run"));
FGameplayTag ASoldier::SkillJumpTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Jump"));
FGameplayTag ASoldier::SkillCrouchTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Crouch"));
FGameplayTag ASoldier::SkillFireWeaponTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon"));
FGameplayTag ASoldier::SkillAreaEffectFromSelfTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.AreaEffectFromSelf"));

ASoldier::ASoldier(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer.SetDefaultSubobjectClass<USoldierMovementComponent>(ACharacter::CharacterMovementComponentName)), bAbilitiesInitialized{ false }, bDefaultWeaponsInitialized{ false }
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	initStats();
	initCameras();
	initMovements();
	initMeshes();
	setup_stimulus();
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


void ASoldier::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	//DOREPLIFETIME_CONDITION(ASoldier, Inventory, COND_OwnerOnly);

	// everyone except local owner: flag change is locally instigated

	// everyone
	DOREPLIFETIME(ASoldier, PlayerTeam);
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
	// TODO: Link with attribut set (when possible)
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
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

void ASoldier::InitializeAttributes()
{
	check(AbilitySystemComponent);

	if (!DefaultAttributeEffects)
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
	AbilitySystemComponent->RegisterGameplayTagEvent(ASoldier::StateDeadTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASoldier::DeadTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(ASoldier::StateRunningTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASoldier::RunningTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(ASoldier::StateJumpingTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASoldier::JumpingTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(ASoldier::StateFightingTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASoldier::FightingTagChanged);
}

void ASoldier::InitializeAttributeChangeCallbacks()
{
	HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ASoldier::HealthChanged);
}

void ASoldier::DeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0) // If dead tag is added - Handle death
	{
		// Stop the soldier and remove any interaction with the world
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->GravityScale = 0.f;
		GetCharacterMovement()->Velocity = FVector(0.f);

		// Cancel abilities
		AbilitySystemComponent->CancelAllAbilities();

		// Notify the death to GameMode - Server only
		if (ASquadLeaderGameModeBase* GameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); GameMode)
			GameMode->SoldierDied(GetController());
	}
	else // If dead tag is removed - Handle respawn
	{
		// A setter is ok for this special case. Otherwise use GEs to handle attributes
		AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
		AttributeSet->SetShield(AttributeSet->GetMaxShield());

		GetCharacterMovement()->GravityScale = 1.f;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ASoldier::RunningTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
}

void ASoldier::JumpingTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
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

void ASoldier::MoveForward(const float _Val)
{
	AddMovementInput(FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X), _Val);
}

void ASoldier::MoveRight(const float _Val)
{
	AddMovementInput(FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y), _Val);
}

void ASoldier::LookUp(const float _Val)
{
	if (IsAlive())
		AddControllerPitchInput(_Val);
}

void ASoldier::Turn(const float _Val)
{
	if (IsAlive())
		AddControllerYawInput(_Val);
}

// TODO: For now, we directly change the move speed multiplier with a setter. This is should be changed 
// through a GE. It should use the execalculation to consider all the buffs/debbufs
bool ASoldier::StartRunning()
{
	AttributeSet->SetMoveSpeedMultiplier(3.f);
	return true;
}

void ASoldier::StopRunning()
{
	AttributeSet->SetMoveSpeedMultiplier(1.f);
}

bool ASoldier::Walk()
{
	UnCrouch();
	StopRunning();
	return true;
}

FVector ASoldier::lookingAtPosition()
{
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

float ASoldier::GetMoveSpeedWalk() const
{
	return AttributeSet ? AttributeSet->GetMoveSpeedWalk() : -1.0f;
}

float ASoldier::GetMoveSpeedCrouch() const
{
	return AttributeSet ? AttributeSet->GetMoveSpeedCrouch() : -1.0f;
}

float ASoldier::GetMoveSpeedMultiplier() const
{
	return AttributeSet ? AttributeSet->GetMoveSpeedMultiplier() : -1.0f;
}

void ASoldier::HealthChanged(const FOnAttributeChangeData& _Data)
{
	if (!IsAlive())
		Die();
}

void ASoldier::Die()
{
	// Give dead tag - death will be handled in DeadTagChanged
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle DeathHandle = AbilitySystemComponent->MakeOutgoingSpec(UGE_StateDead::StaticClass(), 1.f, EffectContext);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*DeathHandle.Data.Get());
}

void ASoldier::Respawn()
{
	// Remove dead tag - respawn will be handled in DeadTagChanged
	FGameplayTagContainer EffectTagsToRemove;
	EffectTagsToRemove.AddTag(ASoldier::StateFightingTag); // Make sure all passive are available on respawn
	EffectTagsToRemove.AddTag(ASoldier::StateDeadTag);
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(EffectTagsToRemove);
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

void ASoldier::ServerChangeTeam_Implementation(ENUM_PlayerTeam _PlayerTeam)
{
	PlayerTeam = _PlayerTeam;
}

bool ASoldier::ServerChangeTeam_Validate(ENUM_PlayerTeam _PlayerTeam)
{
	return true;
}

void ASoldier::OnRep_ChangeTeam()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerChangeTeam(PlayerTeam);
	}
}

void ASoldier::cycleBetweenTeam()
{
	FString message;
	switch (PlayerTeam)
	{
	case ENUM_PlayerTeam::None:
		PlayerTeam = ENUM_PlayerTeam::Team1;
		message = FString::Printf(TEXT("You now are in team1"));
		break;
	case ENUM_PlayerTeam::Team1:
		PlayerTeam = ENUM_PlayerTeam::Team2;
		message = FString::Printf(TEXT("You now are in team2"));
		break;
	case ENUM_PlayerTeam::Team2:
		message = FString::Printf(TEXT("You now are in no team"));
		PlayerTeam = ENUM_PlayerTeam::None;
		break;
	default:
		break;
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, message);

}

void ASoldier::setup_stimulus() {
	stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("stimulusSight"));
	stimulus->RegisterForSense(TSubclassOf <UAISense_Sight>());
	stimulus->RegisterWithPerceptionSystem();
};