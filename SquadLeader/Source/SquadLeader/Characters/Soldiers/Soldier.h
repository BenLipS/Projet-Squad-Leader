#pragma once

#include "Core.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "camera/cameracomponent.h"
#include "UObject/ObjectMacros.h"
#include "Net/UnrealNetwork.h"
#include "Soldier.generated.h"

UENUM()
enum class ENUM_PlayerTeam : uint8 {
	Team1       UMETA(DisplayName = "PlayerTeam1"),
	Team2       UMETA(DisplayName = "PlayerTeam2"),
};

UCLASS()
class SQUADLEADER_API ASoldier : public ACharacter
{
	GENERATED_BODY()

public:
	ASoldier();

protected:
	virtual void BeginPlay() override;

public:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;


//////////////// Inits
private:
	void initCameras();
	void initMeshes();
	void initStats();
	void initMovements();

//////////////// Cameras
private:
	void setToFirstCameraPerson();
	void setToThirdCameraPerson();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* ThirdPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComponent;

protected:
	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Camera")
	bool bIsFirstPerson;

public:
	UFUNCTION()
	void onSwitchCamera();

////////////////  Meshes
public:
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* FirstPersonMesh;

//////////////// Movement
	// Move direction
	UFUNCTION()
	void onMoveForward(const float _val);

	UFUNCTION()
	void onMoveRight(const float _val);

	// Jump
	UFUNCTION()
	void onStartJumping();

	UFUNCTION()
	void onStopJumping();

	// Crouch
	UFUNCTION()
	void onStartCrouching();

	UFUNCTION()
	void onStopCrouching();

	// Run
	UFUNCTION()
	void onStartRunning();

	UFUNCTION()
	void onStopRunning();

protected:
	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Transient, Replicated, Category = "Movement")
	bool bWantsToRun;

	void setRunning(const bool _wantsToRun);

	UFUNCTION(reliable, server, WithValidation)
	void ServerSetRunning(const bool _wantsToRun);

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool isRunning() const noexcept;

////////////////  PlayerCondition
	UPROPERTY(BluePrintReadWrite, Category = "PlayerCondition")
	float fieldOfViewNormal;

	UPROPERTY(BluePrintReadWrite, Category = "PlayerCondition")
	float fieldOfViewAim;

////////////////  PlayerTeam
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "PlayerTeam")
		ENUM_PlayerTeam PlayerTeam;
};
