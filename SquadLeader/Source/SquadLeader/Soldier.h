// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "camera/cameracomponent.h"
#include "Soldier.generated.h"

UCLASS()
class SQUADLEADER_API ASoldier : public ACharacter
{
	GENERATED_BODY()

public:
	ASoldier();

protected:
	virtual void BeginPlay() override;

public:	
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

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Camera")
	bool bIsFirstPerson;

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
	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Movement")
	bool bIsRunning;

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool isRunning() const noexcept;

////////////////  PlayerCondition
	UPROPERTY(BluePrintReadWrite, Category = "PlayerCondition")
	float fieldOfViewNormal;

	UPROPERTY(BluePrintReadWrite, Category = "PlayerCondition")
	float fieldOfViewAim;
};
