#include "SoldierPlayer.h"
#include "SoldierPlayerState.h"
#include "SoldierPlayerController.h"
#include "../../AI/AISquadController.h"
#include "../../SquadLeaderGameModeBase.h"
#include "../../AI/AISquadManager.h"
#include "../../AbilitySystem/Soldiers/GameplayAbilitySoldier.h"
#include "../../Spawn/SoldierSpawn.h"
#include "SquadLeader/UI/Interface/MinimapInterface.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "SquadLeader/Weapons/SL_Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "AkAudioEvent.h"
#include "AkGameplayStatics.h"

#include "AkAudioEvent.h"
#include "AkGameplayStatics.h"

ASoldierPlayer::ASoldierPlayer(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	PostProcessVolume = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessVolume"));
	PostProcessVolume->bUnbound = false;
	PostProcessVolume->bEnabled = true;
	PostProcessVolume->AttachToComponent(ThirdPersonCameraComponent, FAttachmentTransformRules::KeepRelativeTransform);

	InitCameraKiller();
}

/*
* On the Server, Possession happens before BeginPlay.
* On the Client, BeginPlay happens before Possession.
* So we can't use BeginPlay to do anything with the AbilitySystemComponent because we don't have it until the PlayerState replicates from possession.
*/
void ASoldierPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (GetTeam())
		GetTeam()->AddSoldierList(this);

	if (!IsLocallyControlled())
		return;

	UAkGameplayStatics::PostEventByName("Music_Gameplay", this);

	ensure(MaterialGlitchInterface);
	ensure(MaterialBrokenGlassRightInterface);
	ensure(MaterialBrokenGlassLeftInterface);

	// Glitch
	if (MaterialGlitchInterface)
	{
		MaterialGlitchInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), MaterialGlitchInterface);
		SetWeightGlitchEffect(0.f);
	}

	// Broken glass
	if (MaterialBrokenGlassRightInterface && MaterialBrokenGlassLeftInterface)
	{
		MaterialBrokenGlassRightInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), MaterialBrokenGlassRightInterface);
		MaterialBrokenGlassLeftInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), MaterialBrokenGlassLeftInterface);

		PostProcessVolume->AddOrUpdateBlendable(MaterialBrokenGlassRightInstance, 0.f);
		PostProcessVolume->AddOrUpdateBlendable(MaterialBrokenGlassLeftInstance, 0.f);
	}
	// Wall Vision
	if (MaterialWallVisionViewInterface)
	{
		MaterialWallVisionViewInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), MaterialWallVisionViewInterface);
		PostProcessVolume->AddOrUpdateBlendable(MaterialWallVisionViewInstance, 0.f);
	}

	// Blood
	if (MaterialBloodInterface)
	{
		MaterialBloodInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), MaterialBloodInterface);
		PostProcessVolume->AddOrUpdateBlendable(MaterialBloodInstance, 0.f);
	}

	bHitMontageActivated = false;
}

void ASoldierPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())
		return;

	if (SoldierKiller)
	{
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(DeathLocation, SoldierKiller->GetActorLocation());
		FollowKillerCamera->SetWorldRotation(LookAtRotation.Quaternion());
		FollowKillerCamera->SetWorldLocation(DeathLocation); // Force the camera to be to the same position regardless the soldier
	}
}

void ASoldierPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoldierPlayer, SquadManager);
}

// Server only 
void ASoldierPlayer::PossessedBy(AController* _newController)
{
	Super::PossessedBy(_newController);
	SetAbilitySystemComponent();

	InitSquadManager();
}

// Client only 
void ASoldierPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	SetAbilitySystemComponent();

	//-----HUD-----
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetController()); PC)
		PC->CreateHUD();
}

void ASoldierPlayer::DeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::DeadTagChanged(CallbackTag, NewCount);

	if (!IsLocallyControlled())
		return;

	if (NewCount == 0) // Respawn
	{
		HitRight = 0;
		HitLeft = 0;
		UpdateBrokenGlassEffect();
		SetWeightGlitchEffect(0.f);

		DeactivateFollowKillerCamera();
		SoldierKiller = nullptr;
	}
	else
	{
		DeathLocation = CurrentCameraComponent->GetComponentLocation();
		FTimerHandle Timer{};
		GetWorldTimerManager().SetTimer(Timer, this, &ASoldierPlayer::ActivateFollowKillerCamera, 2.f, false);
	}
}

void ASoldierPlayer::InitCameraKiller()
{
	FollowKillerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowKillerCamera"));
	FollowKillerCamera->SetupAttachment(GetMesh());
	FollowKillerCamera->SetAbsolute(true, true, true);
	FollowKillerCamera->SetFieldOfView(BaseFOVNormal);
	FollowKillerCamera->Deactivate();
}

void ASoldierPlayer::InitSquadManager()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
		return;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SquadManager = GetWorld()->SpawnActorDeferred<AAISquadManager>(AISquadManagerClass, FTransform{}, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (SquadManager)
	{
		// SquadManager->Init(GetTeam(), this);  // done on the begin play of this element
		SquadManager->FinishSpawning(FTransform{});

		if (ASquadLeaderGameModeBase* GM = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); GM)
			GM->ListAISquadManagers.Add(SquadManager);
	}
}

void ASoldierPlayer::OnBlurredVisionFromJammer(const bool _IsBlurred)
{
	if (!IsLocallyControlled())
		return;

	if (_IsBlurred) // Apply the glitches
		SetWeightGlitchEffect(1.f);
	else if (WeightGlitchEffect >= 0.1f)
		StartReducingGlitch();
	else
		EndGlitch();
}

void ASoldierPlayer::StartReducingGlitch()
{
	GetWorldTimerManager().SetTimer(TimerGlitchReduction, this, &ASoldierPlayer::ReduceGlitch, TimeBetweenReductionGlitch, true);
	ReduceGlitch();
}

void ASoldierPlayer::ReduceGlitch()
{
	if (const float NewWeight = WeightGlitchEffect * ReductionMultiplierGlitch; NewWeight >= MinimumWeightForGlitchReduction)
		SetWeightGlitchEffect(NewWeight);
	else
		EndGlitch();
}

void ASoldierPlayer::EndGlitch()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerGlitchReduction);
	SetWeightGlitchEffect(0.f);
}

UCameraComponent* ASoldierPlayer::GetFollowKillerCamera() const
{
	return FollowKillerCamera;
}

void ASoldierPlayer::ActivateFollowKillerCamera()
{
	if (!SoldierKiller)
		return;

	// Allow to see the killer through walls
	SoldierKiller->GetMesh()->SetRenderCustomDepth(true);
	
	FollowKillerCamera->SetWorldLocation(DeathLocation);
	CurrentCameraComponent->Deactivate();
	FollowKillerCamera->Activate();
}

void ASoldierPlayer::DeactivateFollowKillerCamera()
{
	if (SoldierKiller)
		SoldierKiller->GetMesh()->SetRenderCustomDepth(false);

	FollowKillerCamera->Deactivate();
	CurrentCameraComponent->Activate();
}

void ASoldierPlayer::SetSoldierKiller(ASoldier* _SoldierKiller)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && !IsLocallyControlled())
		ClientSetSoldierKiller(_SoldierKiller);

	SoldierKiller = _SoldierKiller;
}

void ASoldierPlayer::ClientSetSoldierKiller_Implementation(ASoldier* _SoldierKiller)
{
	SetSoldierKiller(_SoldierKiller);
}

void ASoldierPlayer::LockControls()
{
	if (APlayerController* PC = Cast<APlayerController>(Controller); PC)
		PC->DisableInput(PC);
}

void ASoldierPlayer::UnLockControls()
{
	if (APlayerController* PC = Cast<APlayerController>(Controller); PC)
		PC->EnableInput(PC);
}

AAISquadManager* ASoldierPlayer::GetSquadManager()
{
	return SquadManager;
}

void ASoldierPlayer::OnRep_SquadManager()
{
	if (SquadManager)
	{
		// Remove soldier from team then re-add them - This is not the cleanest nor the most optimize solution but it works
		for (ASoldierAI* Soldier : SquadManager->GetAISoldierList())
		{
			if (Soldier)
				Soldier->RefreshTeam();
		}
	}
}

void ASoldierPlayer::LookUp(const float _Val)
{
	Super::LookUp(_Val);

	if (IsLocallyControlled())
	{
		if (HasAuthority())
			MulticastSyncControlRotation(SyncControlRotation);
		else
			ServerSyncControlRotation(SyncControlRotation);
	}
}

void ASoldierPlayer::Turn(const float _Val)
{
	Super::Turn(_Val);

	if (IsLocallyControlled())
	{
		if (HasAuthority())
			MulticastSyncControlRotation(SyncControlRotation);
		else
			ServerSyncControlRotation(SyncControlRotation);
	}
}

void ASoldierPlayer::SetWeightGlitchEffect(const float _Weight)
{
	WeightGlitchEffect = _Weight;
	PostProcessVolume->AddOrUpdateBlendable(MaterialGlitchInstance, _Weight);
}

void ASoldierPlayer::ActivateHitMontage()
{
	bHitMontageActivated = true;

	// Wait a bit before enabling the hit montage again - So the montage can't be spammed
	FTimerHandle TimerHitReactMontage{};
	GetWorldTimerManager().SetTimer(TimerHitReactMontage, this, &ASoldierPlayer::DisableHitMontage, FMath::RandRange(0.75f, 1.25f), false);
}

void ASoldierPlayer::DisableHitMontage()
{
	bHitMontageActivated = false;
}

void ASoldierPlayer::StartHitReactMontage(UAnimMontage* _HitReactMontage)
{
	PlayAnimMontage(_HitReactMontage);
	ActivateHitMontage();
}

void ASoldierPlayer::SetAbilitySystemComponent()
{
	check(IsValid(GetPlayerState()))

	if (ASoldierPlayerState* PS = Cast<ASoldierPlayerState>(GetPlayerState()); PS)
	{
		AbilitySystemComponent = PS->GetAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		AttributeSet = PS->GetAttributeSet();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeTagChangeCallbacks();
		InitializeAttributeChangeCallbacks();
		InitializeAttributes();
		InitializeAbilities();
		AddStartupEffects();
		BindASCInput();
	}
}

void ASoldierPlayer::BindASCInput()
{
	ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetController());

	if (!PC)
		return;

	UInputComponent* inputComponent = PC->InputComponent;

	if (!ASCInputBound && AbilitySystemComponent && IsValid(inputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(inputComponent, FGameplayAbilityInputBinds(FString("ConfirmSkill"),
			FString("CancelSkill"), FString("ESoldierAbilityInputID"), static_cast<int32>(ESoldierAbilityInputID::ConfirmSkill), static_cast<int32>(ESoldierAbilityInputID::CancelSkill)));

		ASCInputBound = true;
	}
}

void ASoldierPlayer::cycleBetweenTeam()
{
	if (GetLocalRole() == ROLE_Authority) {
		Super::cycleBetweenTeam();
		SquadManager->UpdateSquadTeam(GetTeam());
	}
	else ServerCycleBetweenTeam();
}

void ASoldierPlayer::SpawnClientPing_Implementation(FVector2D ActorLocationIn)
{
	if (ASoldierPlayerController* PC = GetController<ASoldierPlayerController>(); PC)
	{
		if (auto HUD = PC->GetHUD<IMinimapInterface>(); HUD)
		{
			HUD->OnPingAdded(ActorLocationIn);
		}
	}
}

void ASoldierPlayer::SpawnPing(FVector PingLocation)
{
	FTransform PingTransform;
	PingTransform.SetLocation(PingLocation);
	if (PingMesh) 
		PingMesh->Destroy();
	
	PingMesh = GetWorld()->SpawnActorDeferred<AActor>(PingClass, PingTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (PingMesh)
	{
		PingMesh->FinishSpawning(PingTransform);

		if (ASoldierPlayerController* PC = GetController<ASoldierPlayerController>(); PC)
		{
			if (auto HUD = PC->GetHUD<IMinimapInterface>(); HUD)
			{
				HUD->OnPingAdded({ PingMesh->GetActorLocation().X, PingMesh->GetActorLocation().Y });
			}
		}
		SpawnClientPing({ PingMesh->GetActorLocation().X, PingMesh->GetActorLocation().Y });
		UAkGameplayStatics::PostEventByName("Ping_Notif_Good", this);
	}
}

void ASoldierPlayer::DestroyPing() {
	if (PingMesh)
	{
		PingMesh->Destroy();
		if (ASoldierPlayerController* PC = GetController<ASoldierPlayerController>(); PC)
		{
			if (auto HUD = PC->GetHUD<IMinimapInterface>(); HUD)
			{
				HUD->OnPingDestroyed();
			}
		}
		DestroyClientPing();
	}
}

void ASoldierPlayer::DestroyClientPing_Implementation()
{
	if (ASoldierPlayerController* PC = GetController<ASoldierPlayerController>(); PC)
	{
		if (auto HUD = PC->GetHUD<IMinimapInterface>(); HUD)
		{
			HUD->OnPingDestroyed();
		}
	}
}

void ASoldierPlayer::LevelUp()
{
	Super::LevelUp();

	if (ASoldierPlayerController* PC = GetController<ASoldierPlayerController>(); PC)
	{
		for (int i = 0; i < NbAIsForNextLevelUp.GetValueAtLevel(GetCharacterLevel()); ++i)
			PC->AddAnAIToIndexSquad();
	}
}

FVector ASoldierPlayer::GetRespawnPoint()
{
	if (GetTeam()) {
		auto AvailableSpawnPoints = GetTeam()->GetUsableSpawnPoints();
		if (AvailableSpawnPoints.Num() > 0) {

			ASoldierSpawn* OptimalSpawn = AvailableSpawnPoints[0];
			auto CalculateMinimalDistance = [](FVector PlayerPos, FVector FirstPoint, FVector SecondPoint) {  // return true if the first point is closest
				float dist1 = FVector::Dist(PlayerPos, FirstPoint);
				float dist2 = FVector::Dist(PlayerPos, SecondPoint);
				return dist1 < dist2;
			};

			for (auto loop : AvailableSpawnPoints) {
				if (CalculateMinimalDistance(this->GetActorLocation(), loop->GetActorLocation(), OptimalSpawn->GetActorLocation())) {
					OptimalSpawn = loop;
				}
			}

			return OptimalSpawn->GetSpawnLocation();
		}
	}
	return FVector(200.f, 200.f, 1500.f); // else return default
}

void ASoldierPlayer::OnSquadChanged(const TArray<FSoldierAIData>& newValue)
{
	if (ASoldierPlayerController* PC = GetController<ASoldierPlayerController>(); PC)
	{
		PC->OnSquadChanged(newValue);
	}
}

void ASoldierPlayer::OnReceiveDamage(const FVector& _ImpactPoint, const FVector& _SourcePoint)
{
	if (!IsLocallyControlled())
	{
		if (GetLocalRole() == ROLE_Authority)
			ClientOnReceiveDamage(_ImpactPoint, _SourcePoint);
		return;
	}

	const FVector ShootDir = FVector{ _ImpactPoint.X - _SourcePoint.X, _ImpactPoint.Y - _SourcePoint.Y, 0.f }.GetSafeNormal();
	if (ShootDir.IsNearlyZero())
		return;

	const float ForwardCosAngle = FVector::DotProduct(GetActorForwardVector(), -ShootDir);
	const float RightCosAngle = FVector::DotProduct(GetActorRightVector(), -ShootDir);

	//if (GEngine) GEngine->AddOnScreenDebugMessage(33, 1.f, FColor::Green, FString::Printf(TEXT("Forward %f"), FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle))));
	//if (GEngine) GEngine->AddOnScreenDebugMessage(44, 1.f, FColor::Green, FString::Printf(TEXT("Right %f"), FMath::RadiansToDegrees(FMath::Acos(RightCosAngle))));

	const bool bHitOnright = RightCosAngle >= 0.f;
	const bool bHitOnFront = ForwardCosAngle >= PI * 0.25f;
	const bool bHitOnBack = ForwardCosAngle <= PI * -0.25f;
	const UAnimMontage* FireMontage = GetCurrentWeapon() ? GetCurrentWeapon()->FireMontage : nullptr;

	if (bHitOnright) //Hit right side
	{
		AddBrokenGlassOnRight();

		// Hit react montage is not available if there is another montage (except fire weapon)
		if (bHitMontageActivated || (GetCurrentMontage() && GetCurrentMontage() != FireMontage))
			return;

		if (bHitOnFront && HitReactFrontMontage)
			StartHitReactMontage(HitReactFrontMontage);
		else if (bHitOnBack && HitReactBackMontage)
			StartHitReactMontage(HitReactBackMontage);
		else if (HitReactRightMontage)
			StartHitReactMontage(HitReactRightMontage);
	}
	else //Hit left side
	{
		AddBrokenGlassOnLeft();

		// Hit react montage is not available if there is another montage (except fire weapon)
		if (bHitMontageActivated || (GetCurrentMontage() && GetCurrentMontage() != FireMontage))
			return;

		if (bHitOnFront && HitReactFrontMontage)
			StartHitReactMontage(HitReactFrontMontage);
		else if (bHitOnBack && HitReactBackMontage)
			StartHitReactMontage(HitReactBackMontage);
		else if (HitReactLeftMontage)
			StartHitReactMontage(HitReactLeftMontage);
	}

	ShakeCamera(CameraShakeReceiveDamageClass);
}

void ASoldierPlayer::ClientOnReceiveDamage_Implementation(const FVector& _ImpactPoint, const FVector& _SourcePoint)
{
	OnReceiveDamage(_ImpactPoint, _SourcePoint);
}

void ASoldierPlayer::HealthChanged(const FOnAttributeChangeData& _Data)
{
	Super::HealthChanged(_Data);

	if (!MaterialBloodInstance)
		return;

	PostProcessVolume->AddOrUpdateBlendable(MaterialBloodInstance, 1.f);
	float BloodIntensity = 0.2f;

	float HealthPourcentage = 1 - (GetMaxHealth() - GetHealth()) / GetMaxHealth();

	if (HealthPourcentage < 0.99) {
		BloodIntensity = 0.1;
	}
	if (HealthPourcentage < 0.5) {
		BloodIntensity = 0.2;
	}
	if (HealthPourcentage < 0.3) {
		BloodIntensity = 0.3;
	}
	
	MaterialBloodInstance->SetScalarParameterValue("RadiusIntensity", BloodIntensity);

	if (HealthPourcentage >= 0.99f) {
		PostProcessVolume->AddOrUpdateBlendable(MaterialBloodInstance, 0.f);
	}
}

float ASoldierPlayer::NbOfHitToPPIntensity(int NbHit) const
{
	switch (NbHit) {
		case 1:
			return 0.2f;
		case 2:
			return 0.8f;
		case 3:
			return 2.f;
		default:
			return 0.f;
	}
}

void ASoldierPlayer::AddBrokenGlassOnLeft()
{
	if (HitLeft < 3) {
		HitLeft++;
		FTimerHandle Timer;
		GetWorldTimerManager().SetTimer(Timer, this, &ASoldierPlayer::RemoveBrokenGlassOnLeft, 5.f, false);
	}
	UpdateBrokenGlassEffect();
}

void ASoldierPlayer::AddBrokenGlassOnRight()
{
	if (HitRight < 3) {
		HitRight++;
		FTimerHandle Timer;
		GetWorldTimerManager().SetTimer(Timer, this, &ASoldierPlayer::RemoveBrokenGlassOnRight, 5.f, false);
	}
	UpdateBrokenGlassEffect();
}

void ASoldierPlayer::RemoveBrokenGlassOnLeft()
{
	if (HitLeft > 0) {
		HitLeft--;
	}
	UpdateBrokenGlassEffect();
}

void ASoldierPlayer::RemoveBrokenGlassOnRight()
{
	if (HitRight > 0) {
		HitRight--;
	}
	UpdateBrokenGlassEffect();
}

void ASoldierPlayer::UpdateBrokenGlassEffect()
{
	if(HitRight > 0)PostProcessVolume->AddOrUpdateBlendable(MaterialBrokenGlassRightInstance, 1.f);
	else PostProcessVolume->AddOrUpdateBlendable(MaterialBrokenGlassRightInstance, 1.f);
	if (HitLeft > 0)PostProcessVolume->AddOrUpdateBlendable(MaterialBrokenGlassLeftInstance, 1.f);
	else PostProcessVolume->AddOrUpdateBlendable(MaterialBrokenGlassLeftInstance, 1.f);

	MaterialBrokenGlassRightInstance->SetScalarParameterValue("Bullet Amount", NbOfHitToPPIntensity(HitRight));
	MaterialBrokenGlassLeftInstance->SetScalarParameterValue("Bullet Amount", NbOfHitToPPIntensity(HitLeft));
}

void ASoldierPlayer::UpdateWallVisionPostEffect(float PostEffectValue) {
	if (!IsLocallyControlled())
	{
		return;
	}
	PostProcessVolume->AddOrUpdateBlendable(MaterialWallVisionViewInstance, PostEffectValue);
}
