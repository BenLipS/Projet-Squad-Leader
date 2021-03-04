#include "SoldierPlayer.h"
#include "SoldierPlayerState.h"
#include "SoldierPlayerController.h"
#include "../../SquadLeaderGameInstance.h"
#include "../../AI/AISquadController.h"
#include "../../AbilitySystem/Soldiers/GameplayAbilitySoldier.h"
#include "../../Spawn/SoldierSpawn.h"

//TODO: rmove next include -> only use for the team init -> only use on temporary debug
#include "../../SquadLeaderGameModeBase.h"

ASoldierPlayer::ASoldierPlayer(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer), ASCInputBound{ false }
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
	//TODO: remove the team init -> only use on temporary debug
	if (auto gameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); gameMode) {
		PlayerTeam = gameMode->SoldierTeamCollection[0];
		if (SquadManager)
			SquadManager->UpdateSquadTeam(PlayerTeam);
	}
}

// Server only 
void ASoldierPlayer::PossessedBy(AController* _newController)
{
	Super::PossessedBy(_newController);
	SetAbilitySystemComponent();
	initWeapons();

	/*Init Squad Manager for this Player*/
	SquadManager = NewObject<UAISquadManager>(this, AISquadManagerClass);
	SquadManager->Init(PlayerTeam,this,GetWorld());
	Cast<USquadLeaderGameInstance>(GetGameInstance())->GetSquadManagers().Add(SquadManager);

	// TODO: Do we need to have the hud in server ?
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetController()); PC)
		PC->createHUD();
}

// Client only 
void ASoldierPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	SetAbilitySystemComponent();
	initWeapons();

	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetController()); PC)
		PC->createHUD();
}

UAISquadManager* ASoldierPlayer::GetSquadManager()
{
	return SquadManager;
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

FVector ASoldierPlayer::GetRespawnPoint()
{
	if (PlayerTeam) {
		auto AvailableSpawnPoints = PlayerTeam.GetDefaultObject()->GetUsableSpawnPoints();
		if (AvailableSpawnPoints.Num() > 0) {

			FVector OptimalPosition = AvailableSpawnPoints[0]->GetActorLocation();
			auto CalculateMinimalDistance = [](FVector PlayerPos, FVector FirstPoint, FVector SecondPoint) {  // return true if the first point is closest
				float dist1 = FVector::Dist(PlayerPos, FirstPoint);
				float dist2 = FVector::Dist(PlayerPos, SecondPoint);
				return dist1 < dist2;
			};

			for (auto loop : AvailableSpawnPoints) {
				if (CalculateMinimalDistance(this->GetActorLocation(), loop->GetActorLocation(), OptimalPosition)) {
					OptimalPosition = loop->GetActorLocation();
				}
			}

			return OptimalPosition;
		}
	}
	return FVector(0.f, 0.f, 1500.f); // else return default
}