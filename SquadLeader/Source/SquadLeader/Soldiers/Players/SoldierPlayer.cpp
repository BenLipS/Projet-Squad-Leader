#include "SoldierPlayer.h"
#include "SoldierPlayerState.h"
#include "SoldierPlayerController.h"
#include "../../AI/AISquadController.h"
#include "../../SquadLeaderGameModeBase.h"
#include "../../AI/AISquadManager.h"
#include "../../AbilitySystem/Soldiers/GameplayAbilitySoldier.h"
#include "../../Spawn/SoldierSpawn.h"
#include "SquadLeader/UI/Interface/MinimapInterface.h"
#include "DrawDebugHelpers.h"

ASoldierPlayer::ASoldierPlayer(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer),
NbAIsForNextLevelUp{ 0.f },
ASCInputBound{ false }
{
}

/*
* On the Server, Possession happens before BeginPlay.
* On the Client, BeginPlay happens before Possession.
* So we can't use BeginPlay to do anything with the AbilitySystemComponent because we don't have it until the PlayerState replicates from possession.
*/
void ASoldierPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Server only 
void ASoldierPlayer::PossessedBy(AController* _newController)
{
	Super::PossessedBy(_newController);
	SetAbilitySystemComponent();

	/*Init Squad Manager for this Player*/

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // La maniere de faire le respawn
	FTransform LocationTemp{ {0.f, -1000.f, 0.f}, {0.f,0.f,0.f} };
	SquadManager = GetWorld()->SpawnActorDeferred<AAISquadManager>(AISquadManagerClass, LocationTemp, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (SquadManager)
	{
		SquadManager->FinishSpawning(LocationTemp);
		SquadManager->Init(GetTeam(), this);
		Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode())->ListAISquadManagers.Add(SquadManager);
	}
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

void ASoldierPlayer::SpawnPing(FVector PingLocation)
{
	FTransform PingTransform;
	PingTransform.SetLocation(PingLocation);
	if (PingMesh) 
		PingMesh->Destroy();
	
	PingMesh = GetWorld()->SpawnActorDeferred<AActor>(PingClass, PingTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (PingMesh) PingMesh->FinishSpawning(PingTransform);
	
	if (ASoldierPlayerController* PC = GetController<ASoldierPlayerController>(); PC)
	{
		if (auto HUD = PC->GetHUD<IMinimapInterface>(); HUD)
		{

		}
	}


	if (ASL_HUD* HUD = GetHUD<ASL_HUD>(); HUD)
	{
		OnPingAdded()
	}
	

	////// HUD
	AddPingToHUD()
	{

	}
}
void ASoldierPlayer::DestroyPing() {
	if(PingMesh) PingMesh->Destroy();
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