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
	void OnTextNotification_Received(const FString& notificationString);
	void OnTextNotification_Received_Implementation(const FString& notificationString);

	UFUNCTION(Server, Reliable)
	void OnOrderGiven(MissionType Order, FVector Pos);
	void OnOrderGiven_Implementation(MissionType Order, FVector Pos);

	UFUNCTION(Server, Reliable)
	void AddAnAIToIndexSquad();
	void AddAnAIToIndexSquad_Implementation();

	UFUNCTION(Exec)
	void Cheat_AddAISquad();

	UFUNCTION()
	void BroadCastManagerData();

//////////////// Cheat
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
};
