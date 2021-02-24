// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "../Spawn/SoldierSpawn.h"
#include "../ControlArea/ControlArea.h"
#include "SoldierTeam.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API ASoldierTeam : public AInfo
{
	GENERATED_BODY()

public:
	ASoldierTeam();

protected:
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "GlobalTeamData")
		FString TeamName;


protected:  // controlArea
	UPROPERTY(EditAnywhere, Replicated, Category = "ControlArea")
		TArray<AControlArea*> takenControlArea;
public:
	UFUNCTION(BlueprintCallable, Category = "ControlArea")
		void AddControlArea(AControlArea* newControlArea);
	UFUNCTION(BlueprintCallable, Category = "ControlArea")
		void RemoveControlArea(AControlArea* newControlArea);
	UFUNCTION(BlueprintCallable, Category = "ControlArea")
		void CleanControlArea();


protected: // Spawn points
	UPROPERTY(EditAnywhere, Replicated, Category = "SpawnPoints")
		TArray<ASoldierSpawn*> mainSpawnPoints;
public:
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
	void AddSpawn(ASoldierSpawn* newSpawn);
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
	void RemoveSpawn(ASoldierSpawn* newSpawn);
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
	void CleanSpawnPoints();
};
