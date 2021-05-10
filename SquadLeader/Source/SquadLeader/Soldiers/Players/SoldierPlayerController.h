#pragma once

#include "Core.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "camera/cameracomponent.h"
#include "../Interface/Teamable.h"
#include "../../AI/AISquadManager.h"
#include "../AIs/SoldierAI.h"
#include "../../AI/Mission.h"
#include "SoldierPlayerController.generated.h"

UCLASS()
class SQUADLEADER_API ASoldierPlayerController : public APlayerController, public ITeamable
{
	GENERATED_BODY()

public:
	ASoldierPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;

public:
	virtual void Tick(float _deltaTime) override;
	virtual void SetupInputComponent() override;

//////////////// Teamable
public:
	virtual ASoldierTeam* GetTeam() override;
	virtual bool SetTeam(ASoldierTeam* _Team) override;

// TODO: are the cameras necessary in controllers ?
//////////////// Cameras
protected:
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* ThirdPersonCameraComponent;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Camera")
	bool bIsFirstPerson;

	UFUNCTION()
	void OnSwitchCamera();

//////////////// UI

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<class AHUD> HUDClass;

public:
	UFUNCTION(Client, Reliable)
	void CreateHUD();

	void BindMainAbilities();
	void NotifyMainAbilityCooldown(const float _Cooldown, const ESoldierAbilityInputID _ID);
	void NotifySoldierHit(const float _Damage, const bool _bIsHeadShot);

//////////////// Movements
protected:
	// Move direction
	UFUNCTION()
	void OnMoveForward(const float _Val);

	UFUNCTION()
	void OnMoveRight(const float _Val);

	UFUNCTION()
	void OnLookUp(const float _Val);

	UFUNCTION()
	void OnTurn(const float _Val);

//////////////// Teams
	UFUNCTION()
	void OnChangeTeam();

public:
///////// Consoles Commands
	UFUNCTION(Client, Reliable)
	void ClientSendCommand(const FString& Cmd, bool bWriteToLog);


	//-----HUD SquadManager-----

protected:
	FAISquadManagerData SquadManagerData;

public:
	UFUNCTION(Client, Reliable)
	void OnSquadChanged(const TArray<FSoldierAIData>& newValue);
	void OnSquadChanged_Implementation(const TArray<FSoldierAIData>& newValue);

	UFUNCTION(Client, Reliable)
	void OnSquadMemberHealthChanged(int index, float newHealth);
	void OnSquadMemberHealthChanged_Implementation(int index, float newHealth);

	UFUNCTION(Client, Reliable)
	void OnSquadMemberMaxHealthChanged(int index, float newMaxHealth);
	void OnSquadMemberMaxHealthChanged_Implementation(int index, float newMaxHealth);

	UFUNCTION(Client, Reliable)
	void OnSquadMemberShieldChanged(int index, float newShield);
	void OnSquadMemberShieldChanged_Implementation(int index, float newShield);

	UFUNCTION(Client, Reliable)
	void OnSquadMemberMaxShieldChanged(int index, float newMaxShield);
	void OnSquadMemberMaxShieldChanged_Implementation(int index, float newMaxShield);

	UFUNCTION(Client, Reliable)
	void OnSquadMemberMissionChanged(int index, AIBasicState newMission);
	void OnSquadMemberMissionChanged_Implementation(int index, AIBasicState newMission);

	UFUNCTION(Client, Reliable)
	void OnSquadMemberClassChanged(int index, SoldierClass newMission);
	void OnSquadMemberClassChanged_Implementation(int index, SoldierClass newClass);

	UFUNCTION(Client, Reliable)
	void OnTextNotification_Received(const FString& notificationString);
	void OnTextNotification_Received_Implementation(const FString& notificationString);

	UFUNCTION(Client, Reliable)
	void OnAllyTicket_Received(int newTicket);
	void OnAllyTicket_Received_Implementation(int newTicket);

	UFUNCTION(Client, Reliable)
	void OnEnnemyTicket_Received(int newTicket);
	void OnEnnemyTicket_Received_Implementation(int newTicket);

	UFUNCTION(Client, Reliable)
	void OnGameEnd(const int MatchResult, float GameDuration, int NbKillAI, int NbKillPlayer, int NbDeathByAI, int NbDeathByPlayer);  // -1 for a loss and 1 for a victory
	void OnGameEnd_Implementation(const int MatchResult, float GameDuration, int NbKillAI, int NbKillPlayer, int NbDeathByAI, int NbDeathByPlayer);

	UFUNCTION(Server, Reliable)
	void OnOrderGiven(MissionType Order, FVector Pos);
	void OnOrderGiven_Implementation(MissionType Order, FVector Pos);

	UFUNCTION()
	void AddAnAIToIndexSquad();

	UFUNCTION(Server, Reliable)
	void ServerAddAnAIToIndexSquad();
	void ServerAddAnAIToIndexSquad_Implementation();

	UFUNCTION()
	void BroadCastManagerData();

	//-----ABILITY WALLVISION-----

	UFUNCTION(Client, Reliable)
	void OnWallVisionActivate();
	void OnWallVisionActivate_Implementation();

	UFUNCTION(Client, Reliable)
	void OnWallVisionDeactivate();
	void OnWallVisionDeactivate_Implementation();

	//----- Cooldown -----

	// Get the cooldown of the ability given its ID - This will take only the first ability that match the ID
	/*UFUNCTION()
	float GetCooldown(const ESoldierAbilityInputID _AbilityID);*/


protected:
	/* Return The Correct PlayerParams Class Client-Side */
	UFUNCTION(Reliable, Client)
	void DeterminePlayerParams();
	virtual void DeterminePlayerParams_Implementation();

	/* Set Pawn Class On Server For This Controller */
	UFUNCTION(Reliable, Server, WithValidation)
	virtual void ServerSetPawn(TSubclassOf<APlayerParam> PlayerParam);
	virtual void ServerSetPawn_Implementation(TSubclassOf<APlayerParam> PlayerParam);
	virtual bool ServerSetPawn_Validate(TSubclassOf<APlayerParam> PlayerParam);

//////////////// Cheat
public:
	UFUNCTION(Exec)
	void Cheat_AddAISquad();

	UFUNCTION(exec)
	void Cheat_SuperSoldier();

	UFUNCTION(Server, Reliable)
	void ServerCheat_SuperSoldier();
	void ServerCheat_SuperSoldier_Implementation();

	UFUNCTION(exec)
	void Cheat_Die();

	UFUNCTION(Server, Reliable)
	void ServerCheat_Die();
	void ServerCheat_Die_Implementation();

	UFUNCTION(exec)
	void Cheat_SuperDamage();

	UFUNCTION(Server, Reliable)
	void ServerCheat_SuperDamage();
	void ServerCheat_SuperDamage_Implementation();

	UFUNCTION(exec)
	void Cheat_LevelUp();

	UFUNCTION(Server, Reliable)
	void ServerCheat_LevelUp();
	void ServerCheat_LevelUp_Implementation();
};
