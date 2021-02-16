#pragma once

#include "Core.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "camera/cameracomponent.h"
#include "../../UI/HUDWidget.h"
#include "SoldierPlayerController.generated.h"

UCLASS()
class SQUADLEADER_API ASoldierPlayerController : public APlayerController
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
	void onSwitchCamera();

//////////////// UI
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<class UHUDWidget> HUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	class UHUDWidget* HUDWidget;

public:
	void createHUD();
	UUserWidget* getHUD() const;

//////////////// Movements
protected:
	// Move direction
	UFUNCTION()
	void onMoveForward(const float _val);

	UFUNCTION()
	void onMoveRight(const float _val);

	/*// Run
	UFUNCTION()
	void onStartRunning();

	UFUNCTION()
	void onStopRunning();*/

	UFUNCTION()
	void onChangeTeam();
};
