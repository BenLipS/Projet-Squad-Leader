#include "SoldierPlayerController.h"
#include "SoldierPlayerState.h"
#include "../Soldier.h"
#include "AbilitySystemComponent.h"
#include "../../UI/PlayerHUD.h"

//TODO: rmove next include -> only use for the team init -> only use on temporary debug
#include "../../GameState/SquadLeaderGameState.h"
#include "../Players/SoldierPlayer.h"
#include "../../AI/AISquadManager.h"

ASoldierPlayerController::ASoldierPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SquadManagerData = FAISquadManagerData();
}

void ASoldierPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CreateHUD();
}

void ASoldierPlayerController::CreateHUD_Implementation()
{
	if (!HUDClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing HUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}
	APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>();
	if (CurrentPlayerHUD)
		return;
	ClientSetHUD(HUDClass);
	if (APlayerHUD* HUD = GetHUD<APlayerHUD>(); HUD)
	{
		if (InputComponent)
		{
			InputComponent->BindAction("GiveOrder", IE_Pressed, HUD, &APlayerHUD::OnOrderInputPressed);
			InputComponent->BindAction("GiveOrder", IE_Released, HUD, &APlayerHUD::OnOrderInputReleased);
		}
	}
}

// Server only
void ASoldierPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ASoldierPlayerState* PS = GetPlayerState<ASoldierPlayerState>(); PS)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);

	//TODO: remove the team init -> only use on temporary debug
	if (auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS) {

		if (GS->GetSoldierTeamCollection().Num() == 0) {  // no team obtainable for now, we need to find one (used when playing as server or if no teams in the map)
			ASoldierTeam* LastTeamObtainable = nullptr;
			for (auto SceneActors : GetWorld()->PersistentLevel->Actors) {
				if (auto SoldierTeam = Cast<ASoldierTeam>(SceneActors); SoldierTeam) {
					LastTeamObtainable = SoldierTeam;
				}
			}
			ensure(LastTeamObtainable);  // if trigger, please place a team in the map
			SetTeam(LastTeamObtainable);
		}
		else SetTeam(GS->GetSoldierTeamCollection()[0]);
		
		if (auto soldier = Cast<ASoldierPlayer>(InPawn); soldier->GetSquadManager()) {
			soldier->GetSquadManager()->UpdateSquadTeam(GetTeam());
		}
	}
	Cast<ASoldierPlayer>(InPawn)->GetSquadManager()->OnSquadChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadChanged);
	Cast<ASoldierPlayer>(InPawn)->GetSquadManager()->OnMemberHealthChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberHealthChanged);
	Cast<ASoldierPlayer>(InPawn)->GetSquadManager()->OnMemberMaxHealthChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberMaxHealthChanged);
	Cast<ASoldierPlayer>(InPawn)->GetSquadManager()->OnMemberShieldChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberShieldChanged);
	Cast<ASoldierPlayer>(InPawn)->GetSquadManager()->OnMemberMaxShieldChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberMaxShieldChanged);
	Cast<ASoldierPlayer>(InPawn)->GetSquadManager()->BroadCastSquadData();
}

void ASoldierPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Soldier is possessed.
	CreateHUD();
	if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>())
	{
		CurrentPlayerHUD->SetPlayerStateLink();
		CurrentPlayerHUD->SetAIStateLink();
	}
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
	InputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ASoldierPlayerController::OnSwitchCamera);

	// Movements
	InputComponent->BindAxis("MoveForward", this, &ASoldierPlayerController::OnMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASoldierPlayerController::OnMoveRight);

	InputComponent->BindAxis("Turn", this, &ASoldierPlayerController::OnTurn);
	InputComponent->BindAxis("LookUp", this, &ASoldierPlayerController::OnLookUp);

	//TODO : change debug bindAction when not need anymore
	InputComponent->BindAction("ChangeTeam", IE_Released, this, &ASoldierPlayerController::OnChangeTeam);
}

ASoldierTeam* ASoldierPlayerController::GetTeam()
{
	if (auto SoldierState = Cast<ASoldierPlayerState>(PlayerState); SoldierState) {
		return SoldierState->GetTeam();
	}
	return nullptr;
}

bool ASoldierPlayerController::SetTeam(ASoldierTeam* _Team)
{
	if (auto SoldierState = Cast<ASoldierPlayerState>(PlayerState); SoldierState) {
		return SoldierState->SetTeam(_Team);
	}
	return false;
}

void ASoldierPlayerController::OnSwitchCamera()
{
	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->onSwitchCamera();
}

void ASoldierPlayerController::OnMoveForward(const float _Val)
{
	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->MoveForward(_Val);
}

void ASoldierPlayerController::OnMoveRight(const float _Val) {

	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->MoveRight(_Val);
}

void ASoldierPlayerController::OnLookUp(const float _Val)
{
	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->LookUp(_Val);
}

void ASoldierPlayerController::OnTurn(const float _Val)
{
	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->Turn(_Val);
}

void ASoldierPlayerController::OnChangeTeam()
{
	if (ASoldier* Soldier = Cast<ASoldier>(K2_GetPawn()); Soldier)
		Soldier->cycleBetweenTeam();
}


void ASoldierPlayerController::ClientSendCommand_Implementation(const FString& Cmd, bool bWriteToLog)
{
	ConsoleCommand(Cmd, bWriteToLog);
}

void ASoldierPlayerController::OnSquadChanged_Implementation(const TArray<FSoldierAIData>& newValue)
{
	SquadManagerData.OnSquadDataChanged(newValue);
	if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>(); CurrentPlayerHUD)
		CurrentPlayerHUD->OnSquadChanged(newValue);
}

void ASoldierPlayerController::OnSquadMemberHealthChanged_Implementation(int index, float newHealth)
{
	if (SquadManagerData.SquadData.IsValidIndex(index))
	{
		SquadManagerData.SquadData[index].Health = newHealth;
		//Appel au HUD
		if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>(); CurrentPlayerHUD)
		{
			CurrentPlayerHUD->OnSquadHealthChanged(index, newHealth);
		}
	}
	// Erreur syncronisation client / serveur
}

void ASoldierPlayerController::OnSquadMemberMaxHealthChanged_Implementation(int index, float newMaxHealth)
{
	if (SquadManagerData.SquadData.IsValidIndex(index))
	{
		SquadManagerData.SquadData[index].MaxHealth = newMaxHealth;
		//Appel au HUD
		if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>(); CurrentPlayerHUD)
		{
			CurrentPlayerHUD->OnSquadMaxHealthChanged(index, newMaxHealth);
		}
	}
	// Erreur syncronisation client / serveur
}

void ASoldierPlayerController::OnSquadMemberShieldChanged_Implementation(int index, float newShield)
{
	if (SquadManagerData.SquadData.IsValidIndex(index))
	{
		SquadManagerData.SquadData[index].Shield = newShield;
		//Appel au HUD
		if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>(); CurrentPlayerHUD)
		{
			CurrentPlayerHUD->OnSquadShieldChanged(index, newShield);
		}
	}
	// Erreur syncronisation client / serveur
}

void ASoldierPlayerController::OnSquadMemberMaxShieldChanged_Implementation(int index, float newMaxShield)
{
	if (SquadManagerData.SquadData.IsValidIndex(index))
	{
		SquadManagerData.SquadData[index].MaxShield = newMaxShield;
		//Appel au HUD
		if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>(); CurrentPlayerHUD)
		{
			CurrentPlayerHUD->OnSquadMaxShieldChanged(index, newMaxShield);
		}
	}
	// Erreur syncronisation client / serveur
}

void ASoldierPlayerController::OnOrderGiven_Implementation(MissionType Order, FVector Pos)
{
	if (ASoldierPlayer* Soldier = GetPawn<ASoldierPlayer>(); Soldier)
	{
		if (AAISquadManager* SquadManager = Soldier->GetSquadManager(); SquadManager)
		{
			SquadManager->UpdateMission(Order, Pos);
		}
	}
}

void ASoldierPlayerController::BroadCastManagerData()
{
	if (APlayerHUD* CurrentPlayerHUD = GetHUD<APlayerHUD>(); CurrentPlayerHUD)
		CurrentPlayerHUD->OnSquadChanged(SquadManagerData.SquadData);
}