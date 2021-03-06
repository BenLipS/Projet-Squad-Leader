#include "Soldier.h"
#include "SoldierMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "../SquadLeaderGameModeBase.h"
#include "../AbilitySystem/Soldiers/GameplayAbilitySoldier.h"
#include "../AbilitySystem/Soldiers/GameplayEffects/States/GE_StateDead.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "../SquadLeaderGameModeBase.h"
#include "Kismet/KismetMathLibrary.h"
//#include "DrawDebugHelpers.h"

// States
FGameplayTag ASoldier::StateDeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
FGameplayTag ASoldier::StateRunningTag = FGameplayTag::RequestGameplayTag(FName("State.Running"));
FGameplayTag ASoldier::StateJumpingTag = FGameplayTag::RequestGameplayTag(FName("State.Jumping"));
FGameplayTag ASoldier::StateCrouchingTag = FGameplayTag::RequestGameplayTag(FName("State.Crouching"));
FGameplayTag ASoldier::StateFightingTag = FGameplayTag::RequestGameplayTag(FName("State.Fighting"));
FGameplayTag ASoldier::StateAimingTag = FGameplayTag::RequestGameplayTag(FName("State.Aiming"));
FGameplayTag ASoldier::StateGivingOrderTag = FGameplayTag::RequestGameplayTag(FName("State.GivingOrder"));
FGameplayTag ASoldier::StateReloadingWeaponTag = FGameplayTag::RequestGameplayTag(FName("State.ReloadingWeapon"));

// Abilities
FGameplayTag ASoldier::SkillRunTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Run"));
FGameplayTag ASoldier::SkillJumpTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Jump"));
FGameplayTag ASoldier::SkillCrouchTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Crouch"));
FGameplayTag ASoldier::SkillFireWeaponTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon"));
FGameplayTag ASoldier::SkillGrenadeTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Grenade"));
FGameplayTag ASoldier::SkillAimTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Aim"));
FGameplayTag ASoldier::SkillAreaEffectFromSelfTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.AreaEffectFromSelf"));
FGameplayTag ASoldier::SkillGiveOrderTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.GiveOrder"));
FGameplayTag ASoldier::SkillReloadWeaponTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.ReloadWeapon"));

ASoldier::ASoldier(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer.SetDefaultSubobjectClass<USoldierMovementComponent>(ACharacter::CharacterMovementComponentName)), bAbilitiesInitialized{ false }, bDefaultWeaponsInitialized{ false }
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	initCameras();
	initMovements();
	initMeshes();
	setup_stimulus();
	GetCapsuleComponent()->BodyInstance.SetObjectType(ECC_Player);
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

	if (GetLocalRole() == ROLE_Authority) {
		// add this to the team data
		if (PlayerTeam) {
			OldPlayerTeam = PlayerTeam;
			PlayerTeam.GetDefaultObject()->AddSoldierList(this);
		}
	}
}

void ASoldier::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	//DOREPLIFETIME_CONDITION(ASoldier, Inventory, COND_OwnerOnly);

	// everyone except local owner: flag change is locally instigated

	// everyone
	DOREPLIFETIME(ASoldier, PlayerTeam);
	DOREPLIFETIME(ASoldier, SyncControlRotation);
	//DOREPLIFETIME(AShooterCharacter, CurrentWeapon);
}

void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASoldier::initCameras()
{
	SyncControlRotation = FRotator{0.f, 0.f, 0.f};

	// 1st person camera
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(2.f, 0.f, BaseEyeHeight));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetFieldOfView(90.f);

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
	ThirdPersonCameraComponent->SetFieldOfView(90.f);

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
				AddToInventory(weapon);
				weapon->InitializeAbilitySystemComponent(AbilitySystemComponent);
			}
		}
	}

	if (Inventory.Num() > 0)
		CurrentWeapon = Inventory[0];

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
	AbilitySystemComponent->RegisterGameplayTagEvent(ASoldier::StateAimingTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASoldier::AimingTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(ASoldier::StateGivingOrderTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASoldier::GivingOrderTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(ASoldier::StateReloadingWeaponTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASoldier::ReloadingWeaponTagChanged);
}

void ASoldier::InitializeAttributeChangeCallbacks()
{
	HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ASoldier::HealthChanged);
}

void ASoldier::DeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0) // If dead tag is added - Handle death
	{
		// remove ticket from team (only on server)
		if (PlayerTeam && GetLocalRole() == ROLE_Authority)
			PlayerTeam.GetDefaultObject()->RemoveOneTicket();

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

void ASoldier::AimingTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
}

void ASoldier::GivingOrderTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
}

void ASoldier::ReloadingWeaponTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
}

bool ASoldier::ActivateAbilities(const FGameplayTagContainer& _TagContainer)
{
	return AbilitySystemComponent->TryActivateAbilitiesByTag(_TagContainer);
}

bool ASoldier::ActivateAbility(const FGameplayTag& _Tag)
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(_Tag);
	return AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
}

void ASoldier::CancelAbilities(const FGameplayTagContainer& _TagContainer)
{
	AbilitySystemComponent->CancelAbilities(&_TagContainer);
}

void ASoldier::CancelAbility(const FGameplayTag& _Tag)
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(_Tag);
	AbilitySystemComponent->CancelAbilities(&TagContainer);
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
	AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), _Val);
}

void ASoldier::MoveRight(const float _Val)
{
	AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), _Val);
}

void ASoldier::LookUp(const float _Val)
{
	if (IsAlive())
	{
		AddControllerPitchInput(_Val);
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller); PlayerController)
		{
			SyncControlRotation = PlayerController->GetControlRotation();
		}
	}
}

void ASoldier::Turn(const float _Val)
{
	if (IsAlive())
	{
		AddControllerYawInput(_Val);
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller); PlayerController)
		{
			SyncControlRotation = PlayerController->GetControlRotation();
		}
	}
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

	FVector startLocation = ThirdPersonCameraComponent->GetComponentTransform().GetLocation();
	FVector forwardVector = ThirdPersonCameraComponent->GetForwardVector();
	FVector endLocation = startLocation + forwardVector * 10000.f;

	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(outHit, startLocation, endLocation, ECollisionChannel::ECC_WorldStatic, collisionParams);
	return outHit.bBlockingHit ? outHit.Location : endLocation;
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

FVector ASoldier::GetLocation() const noexcept {
	return GetActorLocation();
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
		CurrentWeapon->TryFiring();
	}
}

void ASoldier::SetWantsToFire(const bool _want, const FGameplayEffectSpecHandle _damageEffectSpecHandle)
{
	wantsToFire = _want;
	if (wantsToFire) {
		CurrentWeapon->TryFiring(_damageEffectSpecHandle);
	}
}

void ASoldier::StartAiming()
{
	FirstPersonCameraComponent->SetFieldOfView(CurrentWeapon->GetFieldOfViewAim());
	ThirdPersonCameraComponent->SetFieldOfView(CurrentWeapon->GetFieldOfViewAim());
}

void ASoldier::StopAiming()
{
	// TODO: Should we have a variable for that ?
	FirstPersonCameraComponent->SetFieldOfView(90.f);
	ThirdPersonCameraComponent->SetFieldOfView(90.f);
}

void ASoldier::ReloadWeapon()
{
	CurrentWeapon->Reload();
}

void ASoldier::OnRep_CurrentWeapon(AWeapon* _LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, _LastWeapon);
}

FRotator ASoldier::GetSyncControlRotation() const noexcept
{
	return SyncControlRotation;
}

void ASoldier::ServerSyncControlRotation_Implementation(const FRotator& _Rotation)
{
	SyncControlRotation = _Rotation;

	if (!IsLocallyControlled())
	{
		FirstPersonCameraComponent->SetWorldRotation(SyncControlRotation);
		ThirdPersonCameraComponent->SetWorldRotation(SyncControlRotation);
	}
}

bool ASoldier::ServerSyncControlRotation_Validate(const FRotator& _Rotation)
{
	return true;
}

void ASoldier::MulticastSyncControlRotation_Implementation(const FRotator& _Rotation)
{
	SyncControlRotation = _Rotation;

	if (!IsLocallyControlled())
	{
		FirstPersonCameraComponent->SetWorldRotation(SyncControlRotation);
		ThirdPersonCameraComponent->SetWorldRotation(SyncControlRotation);
	}
}

bool ASoldier::MulticastSyncControlRotation_Validate(const FRotator& _Rotation)
{
	return true;
}

void ASoldier::AddToInventory(AWeapon* _Weapon)
{
	Inventory.Add(_Weapon);
}

void ASoldier::SetCurrentWeapon(AWeapon* _NewWeapon, AWeapon* _PreviousWeapon)
{
	if (_PreviousWeapon && _NewWeapon !=_PreviousWeapon)
		CurrentWeapon = _NewWeapon;
}

// network for debug team change

void ASoldier::ServerChangeTeam_Implementation(TSubclassOf<ASoldierTeam> _PlayerTeam)
{
	PlayerTeam = _PlayerTeam;
	
	if(OldPlayerTeam)
		OldPlayerTeam.GetDefaultObject()->RemoveSoldierList(this);
	if(PlayerTeam)
		PlayerTeam.GetDefaultObject()->AddSoldierList(this);
	
	OldPlayerTeam = PlayerTeam;
}

bool ASoldier::ServerChangeTeam_Validate(TSubclassOf<ASoldierTeam> _PlayerTeam)
{
	return true;
}

void ASoldier::OnRep_ChangeTeam()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerChangeTeam(PlayerTeam);
	}
}

void ASoldier::ServerCycleBetweenTeam_Implementation() {
	cycleBetweenTeam();
}

bool ASoldier::ServerCycleBetweenTeam_Validate() {
	return true;
}

void ASoldier::cycleBetweenTeam()
{
	if (GetLocalRole() == ROLE_Authority) {
		FString message;
		auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
		auto initialIndex = gameMode->SoldierTeamCollection.Find(PlayerTeam);
		if (initialIndex != INDEX_NONE) {  // cycle between existant team
			auto index = initialIndex + 1;
			if (!(gameMode->SoldierTeamCollection.IsValidIndex(index))) {
				index = 0;
			}
			PlayerTeam = gameMode->SoldierTeamCollection[index];

			message = PlayerTeam.GetDefaultObject()->TeamName;  // Log
		}
		else {  // if the player have no team for now give the first one
			if (gameMode->SoldierTeamCollection.Max() > 0) {
				PlayerTeam = gameMode->SoldierTeamCollection[0];

				message = PlayerTeam.GetDefaultObject()->TeamName;  // Log
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, message);
	}
	else ServerCycleBetweenTeam();
}

void ASoldier::setup_stimulus() {
	stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("stimulusSight"));
	stimulus->RegisterForSense(TSubclassOf <UAISense_Sight>());
	stimulus->RegisterWithPerceptionSystem();
};

uint8 ASoldier::GetInfluenceRadius() const noexcept{
	return InfluenceRadius;
}