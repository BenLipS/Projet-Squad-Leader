#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "camera/cameracomponent.h"
#include "SoldierPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASoldierPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASoldierPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float _deltaTime) override;
	virtual void SetupInputComponent() override;

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

//////////////// Movements
// Move direction
	UFUNCTION()
	void onMoveForward(const float _val);

	UFUNCTION()
	void MoveRight(const float _val);
};
