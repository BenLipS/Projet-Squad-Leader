// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "SoldierSpawn.h"
#include "../Soldiers/SoldierTeam.h"
#include "Net/UnrealNetwork.h"
#include "SoldierPrimarySpawn.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASoldierPrimarySpawn : public ASoldierSpawn
{
	GENERATED_BODY()

public:
	ASoldierPrimarySpawn();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

protected: // Owner part
	// Server call
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerChangeTeamOwner(TSubclassOf<ASoldierTeam> _teamOwner);
	UFUNCTION()
		void OnRep_ChangeTeamOwner();
	TSubclassOf<ASoldierTeam> previousTeamOwner = nullptr;// buffer to know the last owner
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "SpawnCondition")
		TSubclassOf<ASoldierTeam> teamOwner = nullptr;
	UFUNCTION()
		void UpdateTeamOwner();
};
