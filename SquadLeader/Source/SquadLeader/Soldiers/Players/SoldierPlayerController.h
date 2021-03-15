#pragma once

#include "Core.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "camera/cameracomponent.h"
#include "../../UI/HUDWidget.h"
#include "../Interface/Teamable.h"
#include "../../AI/AISquadManager.h"
#include "../AIs/SoldierAI.h"
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
	virtual TSubclassOf<ASoldierTeam> GetTeam() override;
	virtual bool SetTeam(TSubclassOf<ASoldierTeam> _Team) override;

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

	/*UFUNCTION(Reliable, Client, WithValidation)
	void SetRespawnCountdown(const float _RespawnTimeRemaining);
	void SetRespawnCountdown_Implementation(const float _RespawnTimeRemaining);
	bool SetRespawnCountdown_Validate(const float _RespawnTimeRemaining);*/
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

	UFUNCTION()
	void BroadCastManagerData();
};
