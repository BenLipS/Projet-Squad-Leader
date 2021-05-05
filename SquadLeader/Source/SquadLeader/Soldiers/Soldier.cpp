#include "Soldier.h"
#include "SoldierMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "../GameState/SquadLeaderGameState.h"
#include "../SquadLeaderGameModeBase.h"
#include "../AI/InfluenceMap/InfluenceMapGrid.h"
#include "../AbilitySystem/Soldiers/GameplayAbilitySoldier.h"
#include "../AbilitySystem/Soldiers/GameplayEffects/GE_UpdateStats.h"
#include "../AbilitySystem/Soldiers/GameplayEffects/States/GE_StateDead.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "SquadLeader/SquadLeader.h"
#include "SquadLeader/Weapons/SL_Weapon.h"
//#include "DrawDebugHelpers.h"

ASoldier::ASoldier(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer.SetDefaultSubobjectClass<USoldierMovementComponent>(ACharacter::CharacterMovementComponentName)),
bAbilitiesInitialized{ false },
WeaponAttachPointRightHand{ FName("WeaponSocketRightHand") },
WeaponAttachPointLeftHand{ FName("WeaponSocketLeftHand") },
bChangedWeaponLocally{ false },
LevelUpFXRelativeLocation{ FVector{0.f} },
LevelUpFXRotator{ FRotator{} },
LevelUpFXScale{ FVector{1.f} },
ImpactHitFXScale{ FVector{1.f} }
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	InitCameras();
	InitMovements();
	InitMeshes();
	setup_stimulus();

	Inventory = FSoldier_Inventory{};
	CurrentWeaponTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.None"));
}

void ASoldier::Destroyed()
{
	if (GetTeam())
		GetTeam()->RemoveSoldierList(this);
	Super::Destroyed();
}

/*
* On the Server, Possession happens before BeginPlay.
* On the Client, BeginPlay happens before Possession.
* So we can't use BeginPlay to do anything with the AbilitySystemComponent because we don't have it until the PlayerState replicates from possession.
*/
void ASoldier::BeginPlay()
{
	Super::BeginPlay();

	// Cameras
	if (bIsFirstPerson)
	{
		setToFirstCameraPerson();
		onSwitchCamera(); // Force the third person camera. TODO: Make a cleaner organization
	}
	else
		setToThirdCameraPerson();

	CurrentFOV = BaseFOVNormal;

	CacheRelativeTransformMeshInCapsule = GetMesh()->GetRelativeTransform();

	// Teams
	// TODO: Clients must be aware of their team. If we really want a security with the server, we should call this function
	// from the server only, have a test to determine wheter we can change the team, then use a ClientSetTeam to replicate the change
	//if (GetLocalRole() == ROLE_Authority)
	{
		// Add this to the team data or use the default team
		if (GetTeam())
			GetTeam()->AddSoldierList(this);
		else if (InitialTeam)
			SetTeam(InitialTeam);
	}

	if (StartGameMontage)
	{
		LockControls();

		PlayAnimMontage(StartGameMontage);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_SetEndDelegate(StartGame_SoldierMontageEndedDelegate, StartGameMontage);
	}
}

void ASoldier::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetWorldTimerManager().SetTimerForNextTick(this, &ASoldier::SpawnDefaultInventory);
}

void ASoldier::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoldier, Inventory);
	DOREPLIFETIME(ASoldier, SyncControlRotation);
	DOREPLIFETIME(ASoldier, Team);

	// Only replicate CurrentWeapon to simulated clients and manually sync CurrentWeeapon with Owner when we're ready.
	// This allows us to predict weapon changing.
	DOREPLIFETIME_CONDITION(ASoldier, CurrentWeapon, COND_SimulatedOnly);
}

void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*ASquadLeaderGameModeBase* GM = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GetTeam() && GM && GM->InfluenceMap) {
		FGridPackage m_package;
		m_package.m_location_on_map = GetActorLocation();

		ASoldierTeam* team_ = GetTeam();
		if (team_) {
			switch (team_->Id) {
			case 1:
				m_package.team_value = 1;
				break;
			case 2:
				m_package.team_value = 2;
				break;
			default:
				break;
			}
		}

		m_package.m_type = Type::Soldier;
		GM->InfluenceMap->ReceivedMessage(m_package);
	}*/
}

void ASoldier::InitCameras()
{
	SyncControlRotation = FRotator{ 0.f, 0.f, 0.f };

	// 1st person camera
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetMesh());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(2.f, 0.f, BaseEyeHeight));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetFieldOfView(90.f);

	// 3rd person camera
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->TargetArmLength = 170;
	SpringArmComponent->SetRelativeLocation(FVector(-30.f, 40.f, BaseEyeHeight));
	SpringArmComponent->bEnableCameraLag = false;
	SpringArmComponent->bEnableCameraRotationLag = false;
	SpringArmComponent->bUsePawnControlRotation = true;

	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArmComponent);
	ThirdPersonCameraComponent->SetFieldOfView(90.f);

	bIsFirstPerson = true;
	CurrentCameraComponent = FirstPersonCameraComponent;
}

void ASoldier::InitMeshes()
{
	// 1st person mesh
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetOnlyOwnerSee(true); // Only the player sees it
	FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;

	// 3rd person mesh - already defined with ACharacter

	// Montage Delegates
	StartGame_SoldierMontageEndedDelegate.BindUObject(this, &ASoldier::OnStartGameMontageCompleted);
	Respawn_SoldierMontageEndedDelegate.BindUObject(this, &ASoldier::OnRespawnMontageCompleted);
}

void ASoldier::InitMovements()
{
	// TODO: Link with attribut set (when possible)
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// TODO. Review this value
	GetCharacterMovement()->GravityScale = 1.f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
}

void ASoldier::ResetWeapons()
{
	if (GetLocalRole() == ROLE_Authority || GetLocalRole() == ROLE_AutonomousProxy)
	{
		for (ASL_Weapon* Weapon : Inventory.Weapons)
			Weapon->ResetWeapon();
	}
}

void ASoldier::LockControls()
{
}

void ASoldier::UnLockControls()
{
}

UAbilitySystemSoldier* ASoldier::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSetSoldier* ASoldier::GetAttributeSet() const
{
	return AttributeSet;
}

TSubclassOf<UGameplayAbilitySoldier> ASoldier::GetClassAbility1() const
{
	return Ability1;
}

TSubclassOf<UGameplayAbilitySoldier> ASoldier::GetClassAbility2() const
{
	return Ability2;
}

TSubclassOf<UGameplayAbilitySoldier> ASoldier::GetClassAbility3() const
{
	return Ability3;
}

void ASoldier::InitializeAttributes()
{
	check(AbilitySystemComponent);

	if (!StatAttributeEffects)
		return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(StatAttributeEffects, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
	}
}

void ASoldier::InitializeAbilities()
{
	if (GetLocalRole() < ROLE_Authority || bAbilitiesInitialized)
		return;

	check(AbilitySystemComponent);

	InitializeStartupAbilities();
	InitializeMainFightingAbilities();

	bAbilitiesInitialized = true;
}

void ASoldier::InitializeStartupAbilities()
{
	for (TSubclassOf<UGameplayAbilitySoldier>& StartupAbility : StartupAbilities)
	{
		if (!StartupAbility) // Empty element from blueprint
		{
			UE_LOG(LogTemp, Error, TEXT("%s() Invalid ability in StartupAbilities. Check the blueprint of the soldier"), *FString(__FUNCTION__));
			continue;
		}

		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetCharacterLevel(), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}
}

void ASoldier::InitializeMainFightingAbilities()
{
	check(Ability1);
	check(Ability2);
	//check(Ability3);

	// Grant main fighting abilities
	if (Ability1)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability1, GetCharacterLevel(), static_cast<int32>(ESoldierAbilityInputID::Ability1), this));
	}
	else  // Empty element from blueprint
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Invalid Ability1. Check the blueprint of the soldier"), *FString(__FUNCTION__));
	}

	if (Ability2)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability2, GetCharacterLevel(), static_cast<int32>(ESoldierAbilityInputID::Ability2), this));
	}
	else // Empty element from blueprint
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Invalid Ability2. Check the blueprint of the soldier"), *FString(__FUNCTION__));
	}

	if (Ability3)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability3, GetCharacterLevel(), static_cast<int32>(ESoldierAbilityInputID::Ability3), this));
	}
	else // Empty element from blueprint
	{
		//UE_LOG(LogTemp, Error, TEXT("%s() Invalid Ability2. Check the blueprint of the soldier"), *FString(__FUNCTION__));
	}
}

void ASoldier::AddStartupEffects()
{
	check(AbilitySystemComponent);

	if (GetLocalRole() == ROLE_Authority && !AbilitySystemComponent->bStartupEffectsApplied)
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

		AbilitySystemComponent->bStartupEffectsApplied = true;
	}
}

void ASoldier::InitializeTagChangeCallbacks()
{
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Dead")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASoldier::DeadTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Vision.BlurredFromJammer")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASoldier::BlurredFromJammerTagChanged);
}

void ASoldier::InitializeAttributeChangeCallbacks()
{
	HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ASoldier::HealthChanged);
}
TSubclassOf<UGE_UpdateStats> ASoldier::GetStatAttributeEffects() const
{
	return StatAttributeEffects;
}

float ASoldier::GetCooldownRemainingFromAbilityID(const ESoldierAbilityInputID _AbilityID)
{
	return AbilitySystemComponent ? AbilitySystemComponent->GetCooldownRemainingFromAbilityID(_AbilityID) : 0.f;
}

bool ASoldier::IsInCooldown(const FGameplayTag& _Tag)
{
	return AbilitySystemComponent ? AbilitySystemComponent->IsInCooldown(_Tag) : false;
}

void ASoldier::DeadTagChanged(const FGameplayTag _CallbackTag, int32 _NewCount)
{
	if (_NewCount > 0) // If dead tag is added - Handle death
	{
		LockControls();

		// remove ticket from team (only on server)
		if (GetTeam() && GetLocalRole() == ROLE_Authority)
			GetTeam()->RemoveOneTicket();

		// Cancel abilities
		AbilitySystemComponent->CancelAllAbilities();

		// Notify the death to GameMode - Server only
		if (ASquadLeaderGameModeBase* GameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); GameMode)
			GameMode->SoldierDied(GetController());

		// Start ragdoll to the next frame so we can catch all impulses from the capsule before the death - This is useful for the explosion
		//HandleDeathMontage();
		GetWorldTimerManager().SetTimerForNextTick(this, &ASoldier::StartRagdoll);
	}
	else // If dead tag is removed - Handle respawn
	{
		// A setter is ok for this special case. Otherwise use GEs to handle attributes
		AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
		AttributeSet->SetShield(AttributeSet->GetMaxShield());

		ResetWeapons();
		StopRagdoll();

		if (RespawnMontage)
		{
			// Remove any interaction with the world during the respawn animation - avoid damage while the player can't play
			GetCharacterMovement()->Velocity = FVector(0.f);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			PlayAnimMontage(RespawnMontage);
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			AnimInstance->Montage_SetEndDelegate(Respawn_SoldierMontageEndedDelegate, RespawnMontage);
		}
		else
		{
			OnRespawnMontageCompleted(nullptr, false);
		}
	}
}

void ASoldier::BlurredFromJammerTagChanged(const FGameplayTag _CallbackTag, int32 _NewCount)
{
	OnBlurredVisionFromJammer(_NewCount > 0);
}

void ASoldier::OnBlurredVisionFromJammer(const bool _IsBlurred) {

};

bool ASoldier::ActivateAbilities(const FGameplayTagContainer& _TagContainer)
{
	return AbilitySystemComponent->TryActivateAbilitiesByTag(_TagContainer);
}

bool ASoldier::ActivateAbility(const FGameplayTag& _Tag)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(_Tag);
		return AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
	}
	return false;
}

void ASoldier::CancelAbilities(const FGameplayTagContainer& _TagContainer)
{
	if (AbilitySystemComponent)
		AbilitySystemComponent->CancelAbilities(&_TagContainer);
}

void ASoldier::CancelAbility(const FGameplayTag& _Tag)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(_Tag);
		AbilitySystemComponent->CancelAbilities(&TagContainer);
	}
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
	if (_Val != 0.0f)
		AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), _Val);
}

void ASoldier::MoveRight(const float _Val)
{
	if (_Val != 0.0f)
		AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), _Val);
}

void ASoldier::LookUp(const float _Val)
{
	if (_Val != 0.0f)
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
	if (_Val != 0.0f)
	{
		AddControllerYawInput(_Val);
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller); PlayerController)
		{
			SyncControlRotation = PlayerController->GetControlRotation();
		}
	}
}

FVector ASoldier::GetLookingAtPosition(const float _MaxRange) const
{
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ASL_LineTrace), false);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(this);
	Params.bIgnoreBlocks = false;

	FVector ViewStart = GetActorLocation();
	FRotator ViewRot = GetActorRotation();

	if (APlayerController* PC = Cast<APlayerController>(GetController()); PC)
		PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	FVector ViewEnd = ViewStart + (ViewDir * _MaxRange);

	// Get first blocking hit - InstantWeaponFire is a preset for instant fire weapon
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByProfile(HitResult, ViewStart, ViewEnd, FName{"InstantWeaponFire"}, Params);

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Tick for: %s"), *HitResult.Component->GetName()));
	//::DrawDebugLine(GetWorld(), ViewStart, HitResult.bBlockingHit ? HitResult.Location : ViewEnd, FColor::Blue, false, 2.f);

	return HitResult.bBlockingHit ? HitResult.Location : ViewEnd;
}


bool ASoldier::Walk()
{
	UnCrouch();
	StopRunning();
	return true;
}

void ASoldier::Landed(const FHitResult& _Hit)
{
	Super::Landed(_Hit);

	// Force jump ability to end when reaching the ground
	// This is necessary for AIs and players who would keep pressing the input
	CancelAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Jump")));

	if (AbilitySystemComponent)
	{
		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Jumping")));
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(EffectTagsToRemove);
	}
}

bool ASoldier::StartRunning()
{
	AttributeSet->SetMoveSpeedMultiplier(1.8f);

	bIsRunning = true;
	UpdateFOV();

	return true;
}

void ASoldier::StopRunning()
{
	AttributeSet->SetMoveSpeedMultiplier(1.f);

	bIsRunning = false;
	UpdateFOV();
}

bool ASoldier::IsRunning() const noexcept
{
	return bIsRunning && GetCharacterMovement()->Velocity.Size() > 0.0f;
}

void ASoldier::StartAiming()
{
	if (!CurrentWeapon)
		return;

	bIsAiming = true;
	UpdateFOV();
}

void ASoldier::StopAiming()
{
	bIsAiming = false;
	UpdateFOV();
}

bool ASoldier::IsAiming() const noexcept
{
	return bIsAiming;
}

void ASoldier::UpdateFOV()
{
	// Update current FOV
	if (bIsAiming)
		CurrentFOV = CurrentWeapon->GetFieldOfViewAim();
	else if (bIsRunning)
		CurrentFOV = BaseFOVRunning;
	else
		CurrentFOV = BaseFOVNormal;

	ZoomFOVAdd = (CurrentFOV - ThirdPersonCameraComponent->FieldOfView) / TimeFOVAnimation * TimeBetweenFOVChange;

	// Start camera FOV animation
	if (ZoomFOVAdd < 0.f) // Zoom in
	{
		GetWorldTimerManager().SetTimer(TimerFOVAnimation, this, &ASoldier::ZoomInFOV, TimeBetweenFOVChange, true);
		ZoomInFOV();
	}
	else // Zoom out
	{
		GetWorldTimerManager().SetTimer(TimerFOVAnimation, this, &ASoldier::ZoomOutFOV, TimeBetweenFOVChange, true);
		ZoomOutFOV();
	}
}

void ASoldier::ZoomInFOV()
{
	const float NewCameraFOV = ThirdPersonCameraComponent->FieldOfView + ZoomFOVAdd;

	if (NewCameraFOV > CurrentFOV)
	{
		FirstPersonCameraComponent->SetFieldOfView(NewCameraFOV);
		ThirdPersonCameraComponent->SetFieldOfView(NewCameraFOV);
	}
	else
		FinishFOVAnimation();
}

void ASoldier::ZoomOutFOV()
{
	const float NewCameraFOV = ThirdPersonCameraComponent->FieldOfView + ZoomFOVAdd;

	if (NewCameraFOV < CurrentFOV)
	{
		FirstPersonCameraComponent->SetFieldOfView(NewCameraFOV);
		ThirdPersonCameraComponent->SetFieldOfView(NewCameraFOV);
	}
	else
		FinishFOVAnimation();
}

void ASoldier::FinishFOVAnimation()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerFOVAnimation);
	FirstPersonCameraComponent->SetFieldOfView(CurrentFOV);
	ThirdPersonCameraComponent->SetFieldOfView(CurrentFOV);
}

void ASoldier::SpawnDefaultInventory()
{
	if (GetLocalRole() < ROLE_Authority)
		return;

	int32 NumWeaponClasses = DefaultInventoryWeaponClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; ++i)
	{
		if (!DefaultInventoryWeaponClasses[i]) // Empty element from blueprint
		{
			UE_LOG(LogTemp, Error, TEXT("%s() Invalid weapon in DefaultInventoryWeapon. Check the blueprint of the soldier"), *FString(__FUNCTION__));
			continue;
		}

		ASL_Weapon* NewWeapon = GetWorld()->SpawnActorDeferred<ASL_Weapon>(DefaultInventoryWeaponClasses[i],
			FTransform::Identity, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		NewWeapon->FinishSpawning(FTransform::Identity);

		bool bEquipFirstWeapon = (i == 0);
		AddWeaponToInventory(NewWeapon, bEquipFirstWeapon);
	}
}

void ASoldier::OnRep_Inventory()
{
	if (GetLocalRole() == ROLE_AutonomousProxy && Inventory.Weapons.Num() > 0 && !CurrentWeapon)
	{
		// Since we don't replicate the CurrentWeapon to the owning client, this is a way to ask the Server to sync
		// the CurrentWeapon after it's been spawned via replication from the Server.
		// The weapon spawning is replicated but the variable CurrentWeapon is not on the owning client.
		ServerSyncCurrentWeapon();
	}
}

void ASoldier::OnStartFiring()
{
	bIsFiring = true;
}

void ASoldier::OnStopFiring()
{
	bIsFiring = false;
}

bool ASoldier::IsFiring() const noexcept
{
	return bIsFiring;
}

ASL_Weapon* ASoldier::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

TArray<ASL_Weapon*> ASoldier::GetAllWeapons() const
{
	return Inventory.Weapons;
}

void ASoldier::UseCurrentWeaponWithRightHand()
{
	if (!CurrentWeapon->GetWeaponMesh())
	{
		return;
		UE_LOG(LogTemp, Error, TEXT("%s() No mesh on the weapon"), *FString(__FUNCTION__));
	}

	// Get attach point name provided by the weapon. If the name is empty we use the default point from soldier
	const FName AttachPoint = CurrentWeapon->GetRightHandAttachPoint() != NAME_None ? CurrentWeapon->GetRightHandAttachPoint() : WeaponAttachPointRightHand;

	CurrentWeapon->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
}

void ASoldier::UseCurrentWeaponWithLeftHand()
{
	if (!CurrentWeapon->GetWeaponMesh())
	{
		return;
		UE_LOG(LogTemp, Error, TEXT("%s() No mesh on the weapon"), *FString(__FUNCTION__));
	}

	// Get attach point name provided by the weapon. If the name is empty we use the default point from soldier
	const FName AttachPoint = CurrentWeapon->GetLeftHandAttachPoint() != NAME_None ? CurrentWeapon->GetLeftHandAttachPoint() : WeaponAttachPointLeftHand;

	CurrentWeapon->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
}

bool ASoldier::AddWeaponToInventory(ASL_Weapon* _NewWeapon, const bool _bEquipWeapon)
{
	if (!_NewWeapon || GetLocalRole() < ROLE_Authority)
		return false;

	if (_bEquipWeapon)
	{
		// We check if a similar weapon exist to equip it
		if (ASL_Weapon* ExistingWeapon = GetWeaponWithSameClassInInventory(_NewWeapon); ExistingWeapon)
		{
			_NewWeapon->Destroy();
			EquipWeapon(ExistingWeapon);
			ClientSyncCurrentWeapon(CurrentWeapon);
			return true;
		}
		// else we add the new weapon to the inventory - this is done below

	}
	else if (DoesWeaponExistInInventory(_NewWeapon))
	{
		_NewWeapon->Destroy();
		return false;
	}

	Inventory.Weapons.Add(_NewWeapon);
	_NewWeapon->SetOwningSoldier(this);

	if (_bEquipWeapon)
	{
		EquipWeapon(_NewWeapon);
		ClientSyncCurrentWeapon(CurrentWeapon);
	}
	return true;
}

void ASoldier::EquipWeapon(ASL_Weapon* _NewWeapon)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerEquipWeapon(_NewWeapon);
		SetCurrentWeapon(_NewWeapon, CurrentWeapon);
		bChangedWeaponLocally = true;
	}
	else
		SetCurrentWeapon(_NewWeapon, CurrentWeapon);
}

void ASoldier::EquipWeapon(UClass* _WeaponClass)
{
	for (ASL_Weapon* ExistingWeapon : Inventory.Weapons)
	{
		if (ExistingWeapon && ExistingWeapon->GetClass() == _WeaponClass)
		{
			EquipWeapon(ExistingWeapon);
			ClientSyncCurrentWeapon(CurrentWeapon);
		}
	}
}

void ASoldier::ServerEquipWeapon_Implementation(ASL_Weapon* _NewWeapon)
{
	EquipWeapon(_NewWeapon);
}

bool ASoldier::ServerEquipWeapon_Validate(ASL_Weapon* _NewWeapon)
{
	return true;
}

bool ASoldier::DoesWeaponExistInInventory(ASL_Weapon* _Weapon)
{
	return !!GetWeaponWithSameClassInInventory(_Weapon);
}

ASL_Weapon* ASoldier::GetWeaponWithSameClassInInventory(ASL_Weapon* _Weapon)
{
	if (!_Weapon)
		return nullptr;

	for (ASL_Weapon* ExistingWeapon : Inventory.Weapons)
	{
		if (ExistingWeapon && ExistingWeapon->GetClass() == _Weapon->GetClass())
			return ExistingWeapon;
	}
	return nullptr;
}

void ASoldier::SetCurrentWeapon(ASL_Weapon* _NewWeapon, ASL_Weapon* _LastWeapon)
{
	if (_NewWeapon == _LastWeapon)
		return;

	// Cancel active weapon abilities
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer AbilityTagsToCancel = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon")));
		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel);
	}

	UnEquipWeapon(_LastWeapon);
	if (_LastWeapon)
		_LastWeapon->RemoveAbilities();

	if (_NewWeapon)
	{
		// Clear out potential NoWeaponTag
		if (AbilitySystemComponent)
			AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);

		// Weapons coming from OnRep_CurrentWeapon won't have the owner set
		CurrentWeapon = _NewWeapon;
		CurrentWeapon->SetOwningSoldier(this);
		CurrentWeapon->AddAbilities();
		CurrentWeaponTag = CurrentWeapon->WeaponTag;

		if (AbilitySystemComponent)
			AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);

		// Visual part
		UseCurrentWeaponWithRightHand();
		CurrentWeapon->SetActorHiddenInGame(false);
		CurrentWeapon->ForceUpdateAmmo();

		if (CurrentWeapon->GetWeaponMesh())
			CurrentWeapon->GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Reload if the new weapon is empty
		if (!CurrentWeapon->HasAmmo())
			ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.ReloadWeapon")));
	}
}

void ASoldier::UnEquipWeapon(ASL_Weapon* _WeaponToUnEquip)
{
	if (_WeaponToUnEquip)
		_WeaponToUnEquip->SetActorHiddenInGame(true);
}

void ASoldier::OnRep_CurrentWeapon(ASL_Weapon* _LastWeapon)
{
	bChangedWeaponLocally = false;
	SetCurrentWeapon(CurrentWeapon, _LastWeapon);
}

void ASoldier::ServerSyncCurrentWeapon_Implementation()
{
	ClientSyncCurrentWeapon(CurrentWeapon);
}

bool ASoldier::ServerSyncCurrentWeapon_Validate()
{
	return true;
}

void ASoldier::ClientSyncCurrentWeapon_Implementation(ASL_Weapon* _InWeapon)
{
	ASL_Weapon* LastWeapon = CurrentWeapon;
	CurrentWeapon = _InWeapon;
	OnRep_CurrentWeapon(LastWeapon);
}

bool ASoldier::ClientSyncCurrentWeapon_Validate(ASL_Weapon* _InWeapon)
{
	return true;
}

int32 ASoldier::GetCharacterLevel() const
{
	if (AttributeSet)
		return static_cast<int32>(AttributeSet->GetCharacterLevel());
	return -1;
}

float ASoldier::GetEXP() const
{
	return AttributeSet ? AttributeSet->GetEXP() : -1.0f;
}

float ASoldier::GetEXPLevelUp() const
{
	return AttributeSet ? AttributeSet->GetEXPLevelUp() : -1.0f;
}

float ASoldier::GetRemainEXPForLevelUp() const
{
	return AttributeSet ? AttributeSet->GetRemainEXPForLevelUp() : -1.0f;
}

void ASoldier::GrantEXP(const float _EXP)
{
	if (AttributeSet)
		AttributeSet->GrantEXP(_EXP);
}

float ASoldier::GetHealth() const
{
	return AttributeSet ? AttributeSet->GetHealth() : -1.0f;
}

float ASoldier::GetMaxHealth() const
{
	return AttributeSet ? AttributeSet->GetMaxHealth() : -1.0f;
}

float ASoldier::GetShield() const
{
	return AttributeSet ? AttributeSet->GetHealth() : -1.0f;
}

float ASoldier::GetMaxShield() const
{
	return AttributeSet ? AttributeSet->GetMaxShield() : -1.0f;
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

void ASoldier::LevelUp()
{
	AttributeSet->LevelUp();

	if (LevelUpFX && (GetLocalRole() == ROLE_Authority))
		ClientSpawnLevelUpParticle();
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
	EffectTagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Fighting"))); // Make sure all passive are available on respawn
	EffectTagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(EffectTagsToRemove);
}

void ASoldier::OnReceiveDamage(const FVector& _ImpactPoint, const FVector& _SourcePoint)
{
}

TSubclassOf<UMatineeCameraShake> ASoldier::GetCameraShakeFireClass() const
{
	return CameraShakeFireClass;
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

void ASoldier::StartRagdoll()
{
	GetMesh()->SetSimulatePhysics(true);
}

void ASoldier::StopRagdoll()
{
	GetMesh()->SetSimulatePhysics(false);

	// Re-attach the mesh to the capsule 
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	GetMesh()->SetRelativeTransform(CacheRelativeTransformMeshInCapsule);
}

// network for debug team change
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
		if (auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS) {
			auto initialIndex = GS->GetSoldierTeamCollection().Find(GetTeam());
			if (initialIndex != INDEX_NONE) {  // cycle between existant team
				auto index = initialIndex + 1;
				if (!(GS->GetSoldierTeamCollection().IsValidIndex(index))) {
					index = 0;
				}
				SetTeam(GS->GetSoldierTeamCollection()[index]);

				message = GetTeam()->TeamName;  // Log
			}
			else {  // if the player have no team for now give the first one
				if (GS->GetSoldierTeamCollection().Max() > 0) {
					SetTeam(GS->GetSoldierTeamCollection()[0]);

					message = GetTeam()->TeamName;  // Log
				}
			}
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, message);
		}
	}
	else ServerCycleBetweenTeam();
}

ASoldierTeam* ASoldier::GetTeam()
{
	return Team;
}

bool ASoldier::SetTeam(ASoldierTeam* _Team)
{
	// TODO: Clients must be aware of their team. If we really want a security with the server, we should call this function
	// from the server only, have a test to determine wheter we can change the team, then use a ClientSetTeam to replicate the change
	//if (GetLocalRole() == ROLE_Authority)
	{
		Team = _Team;
		return true;
	}
	return false;
}

void ASoldier::setup_stimulus() {
	stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("stimulusSight"));
	stimulus->RegisterForSense(TSubclassOf <UAISense_Sight>());
	stimulus->RegisterWithPerceptionSystem();
};

uint8 ASoldier::GetInfluenceRadius() const noexcept {
	return InfluenceRadius;
}

void ASoldier::ClientSpawnLevelUpParticle_Implementation()
{
	UParticleSystemComponent* LevelUpParticle = UGameplayStatics::SpawnEmitterAttached(LevelUpFX, GetMesh(), FName{ "Pelvis" }, LevelUpFXRelativeLocation, LevelUpFXRotator, EAttachLocation::SnapToTarget);
	LevelUpParticle->SetWorldScale3D(LevelUpFXScale);
}

bool ASoldier::ClientSpawnLevelUpParticle_Validate()
{
	return true;
}

void ASoldier::HandleDeathMontage()
{
	if (DeathMontage)
		PlayAnimMontage(DeathMontage);
}

void ASoldier::OnStartGameMontageCompleted(UAnimMontage* _Montage, bool _bInterrupted)
{
	UnLockControls();
}

void ASoldier::OnRespawnMontageCompleted(UAnimMontage* _Montage, bool _bInterrupted)
{
	UnLockControls();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// TODO: Show particle from the hit location - not center of the soldier
void ASoldier::ShowImpactHitEffect()
{
	UParticleSystemComponent* LaserParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactHitFX, GetActorLocation(), GetActorRotation(), ImpactHitFXScale);
}

FVector ASoldier::GetLookingDirection()
{
	if (CurrentCameraComponent)
	{
		return CurrentCameraComponent->GetForwardVector();
	}
	else
	{
		return GetActorForwardVector();
	}
}
