#include "SoldierPlayerController.h"
#include "SoldierPlayerState.h"
#include "SoldierPlayer.h"
#include "../../SquadLeaderGameInstance.h"
#include "AbilitySystemComponent.h"
#include "SquadLeader/Weapons/SL_Weapon.h"
#include "../SoldierTeam.h"
#include "../../AI/AISquadManager.h"

#include "SquadLeader/UI/HUD/PlayerHUD.h"

#include "SquadLeader/UI/Interface/PlayerHealthInterface.h"
#include "SquadLeader/UI/Interface/PlayerShieldInterface.h"
#include "SquadLeader/UI/Interface/PlayerPrestigeInterface.h"
#include "SquadLeader/UI/Interface/SquadInterface.h"
#include "SquadLeader/UI/Interface/WeaponInterface.h"
#include "SquadLeader/UI/Interface/OrderInterface.h"
#include "SquadLeader/UI/Interface/NotificationInterface.h"
#include "SquadLeader/UI/Interface/MinimapInterface.h"
#include "SquadLeader/UI/Interface/TicketInterface.h"
#include "SquadLeader/UI/Interface/GameEndInterface.h"
#include "SquadLeader/UI/Interface/AbilityCooldownInterface.h"
#include "SquadLeader/UI/Interface/HitMarkerInterface.h"

#include "SquadLeader/SquadLeader.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"


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
	DeterminePlayerParams();
}

void ASoldierPlayerController::CreateHUD_Implementation()
{
	if (!HUDClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing HUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}
	auto CurrentHUD = GetHUD<APlayerHUD>();
	if (CurrentHUD)
		return;
	ClientSetHUD(HUDClass);

	if (auto HUD = GetHUD<APlayerHUD>())
	{
		HUD->InitHUD();

		if (InputComponent)
		{
			InputComponent->BindAction("GiveOrder", IE_Pressed, HUD, &APlayerHUD::OnOrderInputPressed);
			InputComponent->BindAction("GiveOrder", IE_Released, HUD, &APlayerHUD::OnOrderInputReleased);

			InputComponent->BindAction("DisplayMap", IE_Pressed, HUD, &APlayerHUD::OnFullMapDisplayBegin);
			InputComponent->BindAction("DisplayMap", IE_Released, HUD, &APlayerHUD::OnFullMapDisplayEnd);

			InputComponent->BindAction("OpenChat", IE_Pressed, HUD, &APlayerHUD::OnChatInputPressed);
		}
	}
}

void ASoldierPlayerController::BindMainAbilities()
{
	if (auto HUD = GetHUD<IAbilityCooldownInterface>(); HUD)
	{
		HUD->AddAbilityID(ESoldierAbilityInputID::Ability1, "Q");
		HUD->AddAbilityID(ESoldierAbilityInputID::Ability2, "E");
		HUD->AddAbilityID(ESoldierAbilityInputID::Ability3, "F");
	}
}

void ASoldierPlayerController::NotifyMainAbilityCooldown(const float _Cooldown, const ESoldierAbilityInputID _ID)
{
	if (auto HUD = GetHUD<IAbilityCooldownInterface>(); HUD)
		HUD->OnAbilityCooldownTriggered(_Cooldown, _ID);
}

void ASoldierPlayerController::NotifySoldierHit(const float _Damage, const bool _bIsHeadShot)
{
	if (auto HUD = GetHUD<IHitMarkerInterface>(); HUD)
		HUD->OnDamageReceived(_Damage, _bIsHeadShot);
}

void ASoldierPlayerController::ClientDisplayRespawnWidget_Implementation()
{
	APlayerHUD* HUD = GetHUD<APlayerHUD>();
	if (HUD)
		HUD->DisplayRespawnWidget();
}

// Server only
void ASoldierPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ASoldierPlayerState* PS = GetPlayerState<ASoldierPlayerState>(); PS) {
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);

		// re-bind HUD on possess
		CreateHUD();
		if (auto CurrentHUD = GetHUD<APlayerHUD>())
		{
			CurrentHUD->SetPlayerStateLink();
			CurrentHUD->SetAIStateLink();
		}
	}

	ASoldierPlayer* SoldierPlayer = Cast<ASoldierPlayer>(InPawn);
	AAISquadManager* SM = SoldierPlayer->GetSquadManager();
	ensure(SM);

	SM->OnSquadChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadChanged);
	SM->OnMemberHealthChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberHealthChanged);
	SM->OnMemberMaxHealthChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberMaxHealthChanged);
	SM->OnMemberShieldChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberShieldChanged);
	SM->OnMemberMaxShieldChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberMaxShieldChanged);
	SM->OnMemberStateChanged.AddDynamic(this, &ASoldierPlayerController::OnSquadMemberMissionChanged);
	SM->BroadCastSquadData();

	SoldierPlayer->OnRep_SquadManager();
}

void ASoldierPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Soldier is possessed.
	CreateHUD();
	if (auto CurrentHUD = GetHUD<APlayerHUD>())
	{
		CurrentHUD->SetPlayerStateLink();
		CurrentHUD->SetAIStateLink();
	}
}

void ASoldierPlayerController::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	if (auto CurrentHUD = GetHUD<IMinimapInterface>(); CurrentHUD && GetTeam())
		CurrentHUD->OnUpdatePOIs();
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
	if (auto CurrentHUD = GetHUD<ISquadInterface>(); CurrentHUD)
	{
		CurrentHUD->OnSquadChanged(newValue);
	}
}

void ASoldierPlayerController::OnSquadMemberHealthChanged_Implementation(int index, float newHealth)
{
	if (SquadManagerData.SquadData.IsValidIndex(index))
	{
		SquadManagerData.SquadData[index].Health = newHealth;
		//Appel au HUD
		if (auto CurrentHUD = GetHUD<ISquadInterface>(); CurrentHUD)
		{
			CurrentHUD->OnSquadHealthChanged(index, newHealth);
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
		if (auto CurrentHUD = GetHUD<ISquadInterface>(); CurrentHUD)
		{
			CurrentHUD->OnSquadMaxHealthChanged(index, newMaxHealth);
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
		if (auto CurrentHUD = GetHUD<ISquadInterface>(); CurrentHUD)
		{
			CurrentHUD->OnSquadShieldChanged(index, newShield);
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
		if (auto CurrentHUD = GetHUD<ISquadInterface>(); CurrentHUD)
		{
			CurrentHUD->OnSquadMaxShieldChanged(index, newMaxShield);
		}
	}
	// Erreur syncronisation client / serveur
}

void ASoldierPlayerController::OnSquadMemberMissionChanged_Implementation(int index, AIBasicState newMission)
{
	if (SquadManagerData.SquadData.IsValidIndex(index))
	{
		SquadManagerData.SquadData[index].MissionState = newMission;
		//Appel au HUD
		if (auto CurrentHUD = GetHUD<ISquadInterface>(); CurrentHUD)
		{
			CurrentHUD->OnSquadMemberMissionChanged(index, newMission);
		}
	}
	// Erreur syncronisation client / serveur
}

void ASoldierPlayerController::OnSquadMemberClassChanged_Implementation(int index, SoldierClass newClass)
{
	if (SquadManagerData.SquadData.IsValidIndex(index))
	{
		SquadManagerData.SquadData[index].ClassSoldier = newClass;
		//Appel au HUD
		if (auto CurrentHUD = GetHUD<ISquadInterface>(); CurrentHUD)
		{
			CurrentHUD->OnSquadMemberClassChanged(index, newClass);
		}
	}
}

void ASoldierPlayerController::OnTextNotification_Received_Implementation(const FString& notificationString)
{
	if (auto CurrentHUD = GetHUD<INotificationInterface>(); CurrentHUD)
	{
		CurrentHUD->OnTextNotification_Received(notificationString);
	}
}

void ASoldierPlayerController::OnAllyTicket_Received_Implementation(int newTicket)
{
	if (auto HUD = GetHUD<ITicketInterface>(); HUD)
	{
		HUD->OnAllyTicketChanged(newTicket);
	}
}

void ASoldierPlayerController::OnEnnemyTicket_Received_Implementation(int newTicket)
{
	if (auto HUD = GetHUD<ITicketInterface>(); HUD)
	{
		HUD->OnEnnemyTicketChanged(newTicket);
	}
}

void ASoldierPlayerController::OnGameEnd_Implementation(const int MatchResult, float GameDuration, int NbKillAI, int NbKillPlayer, int NbDeathByAI, int NbDeathByPlayer)
{
	if (auto HUD = GetHUD<IGameEndInterface>(); HUD)
	{
		if (MatchResult == 1) HUD->OnGameEnd("VICTORY !");
		else HUD->OnGameEnd("DEFEAT !");
	}

	auto XP = GetPawn<ASoldier>()->GetEXP();
	
	GetGameInstance<USquadLeaderGameInstance>()->UpdateNetworkStatus(MatchResult, GameDuration, XP,
		NbKillAI, NbKillPlayer, NbDeathByAI, NbDeathByPlayer);  // notify the server
}

void ASoldierPlayerController::OnChatMessageReceived_Implementation(const FString& message)
{
	if (auto HUD = GetHUD<IChatInterface>())
	{
		HUD->OnChatMessageReceived(message);
	}
}

void ASoldierPlayerController::OnChatMessageSent_Implementation(const FString& message)
{
	for (auto iterator = GetWorld()->GetPlayerControllerIterator(); iterator; iterator++)
	{
		if (auto PC = Cast<ASoldierPlayerController>(iterator->Get()))
		{
			PC->OnChatMessageReceived(message);
		}
	}
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

void ASoldierPlayerController::AddAnAIToIndexSquad()
{
	if (GetLocalRole() < ROLE_Authority)
		ServerAddAnAIToIndexSquad_Implementation();

	if (Cast<ASoldierPlayer>(GetPawn())->GetSquadManager())
		Cast<ASoldierPlayer>(GetPawn())->GetSquadManager()->AddAnAIToSquad();
}

void ASoldierPlayerController::ServerAddAnAIToIndexSquad_Implementation()
{
	AddAnAIToIndexSquad();
}

void ASoldierPlayerController::BroadCastManagerData()
{
	if (auto CurrentHUD = GetHUD<ISquadInterface>(); CurrentHUD)
		CurrentHUD->OnSquadChanged(SquadManagerData.SquadData);
}

// TODO: Use the soldier list - Don't use all the actors from the world
void ASoldierPlayerController::OnWallVisionActivate_Implementation()
{
	bool Active = false;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASoldier::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (ASoldier* Soldier = Cast<ASoldier>(Actor); Soldier)
			if (Soldier->GetTeam() != GetTeam()) {
				Soldier->GetMesh()->SetRenderCustomDepth(true);
				Active = true;
			}
	}
	if (!Active) {
		FTimerHandle Timer;
		GetWorldTimerManager().SetTimer(Timer, this, &ASoldierPlayerController::OnWallVisionActivate_Implementation, 2.f, false);
	}
}

void ASoldierPlayerController::OnWallVisionDeactivate_Implementation()
{
	for (AActor* Actor : GetWorld()->PersistentLevel->Actors)
	{
		if (ASoldier* Soldier = Cast<ASoldier>(Actor); Soldier)
			Soldier->GetMesh()->SetRenderCustomDepth(false);
	}
}

//float ASoldierPlayerController::GetCooldown(const ESoldierAbilityInputID _AbilityID) const
//{
//
//}


// Pawn Class
void ASoldierPlayerController::DeterminePlayerParams_Implementation()
{
	if (IsLocalController()) //Only Do This Locally (NOT Client-Only, since Server wants this too!)
	{
		ServerSetPawn(GetGameInstance<USquadLeaderGameInstance>()->PlayerParam);
	}
}

bool ASoldierPlayerController::ServerSetPawn_Validate(TSubclassOf<APlayerParam> PlayerParam)
{
	return true;
}

void ASoldierPlayerController::ServerSetPawn_Implementation(TSubclassOf<APlayerParam> PlayerParam)
{
	GetPlayerState<ASoldierPlayerState>()->SetPlayerParam(PlayerParam, this);
}


void ASoldierPlayerController::Cheat_AddAISquad()
{
	AddAnAIToIndexSquad();
}

void ASoldierPlayerController::Cheat_SuperSoldier()
{
	if (GetLocalRole() < ROLE_Authority)
		ServerCheat_SuperSoldier();

	if (ASoldierPlayer* Soldier = GetPawn<ASoldierPlayer>(); Soldier)
	{
		const float badassValue = 9'999'999.f;

		Soldier->GetAttributeSet()->SetMaxHealth(badassValue);
		Soldier->GetAttributeSet()->SetHealth(badassValue);
		Soldier->GetAttributeSet()->SetMaxShield(badassValue);
		Soldier->GetAttributeSet()->SetShield(badassValue);
		Soldier->GetAttributeSet()->SetMoveSpeedWalk(1000.f);
		Soldier->GetAttributeSet()->SetMoveSpeedCrouch(1000.f);
		
		if (ASL_Weapon* Weapon = Soldier->GetCurrentWeapon(); Weapon)
		{
			Weapon->ReloadWeapon();
			Weapon->SetWeaponDamage(badassValue);
			Weapon->SetHasInfiniteAmmo(true);
		}
	}
}

void ASoldierPlayerController::ServerCheat_SuperSoldier_Implementation()
{
	Cheat_SuperSoldier();
}

void ASoldierPlayerController::Cheat_Die()
{
	if (GetLocalRole() < ROLE_Authority)
		ServerCheat_Die();

	if (ASoldierPlayer* Soldier = GetPawn<ASoldierPlayer>(); Soldier)
		Soldier->GetAttributeSet()->SetHealth(0.f);
}

void ASoldierPlayerController::ServerCheat_Die_Implementation()
{
	Cheat_Die();
}

void ASoldierPlayerController::Cheat_SuperDamage()
{
	if (GetLocalRole() < ROLE_Authority)
		ServerCheat_SuperDamage();
	
	if (ASoldierPlayer* Soldier = GetPawn<ASoldierPlayer>(); Soldier && Soldier->GetCurrentWeapon())
		Soldier->GetCurrentWeapon()->SetWeaponDamage(999999.f);
}

void ASoldierPlayerController::ServerCheat_SuperDamage_Implementation()
{
	Cheat_SuperDamage();
}

void ASoldierPlayerController::Cheat_LevelUp()
{
	if (GetLocalRole() < ROLE_Authority)
		ServerCheat_LevelUp();

	if (ASoldierPlayer* Soldier = GetPawn<ASoldierPlayer>(); Soldier)
		Soldier->GrantEXP(Soldier->GetRemainEXPForLevelUp());
}

void ASoldierPlayerController::ServerCheat_LevelUp_Implementation()
{
	Cheat_LevelUp();
}